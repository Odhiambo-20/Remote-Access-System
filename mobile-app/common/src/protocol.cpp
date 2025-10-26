#include "../include/protocol.h"
#include "../include/crypto.h"

using namespace RemoteAccessSystem::Common;

Protocol::Protocol() {
}

uint32_t Protocol::calculateChecksum(const std::vector<unsigned char>& data) {
    uint32_t checksum = 0;
    for (unsigned char byte : data) {
        checksum += byte;
    }
    return checksum;
}

bool Protocol::SendMessage(ACE_SOCK_Stream& socket, const Message& message,
                          const std::vector<unsigned char>& key,
                          const std::vector<unsigned char>& iv) {
    auto data = message.serialize();
    auto encrypted = Crypto::encrypt(data, key, iv);
    
    uint32_t size = encrypted.size();
    if (socket.send_n(&size, sizeof(size)) != sizeof(size)) {
        return false;
    }
    
    return socket.send_n(encrypted.data(), encrypted.size()) == static_cast<ssize_t>(encrypted.size());
}

bool Protocol::ReceiveMessage(ACE_SOCK_Stream& socket, Message& message,
                             const std::vector<unsigned char>& key,
                             const std::vector<unsigned char>& iv) {
    uint32_t size = 0;
    if (socket.recv_n(&size, sizeof(size)) != sizeof(size)) {
        return false;
    }
    
    if (size > 10 * 1024 * 1024) {
        return false;
    }
    
    std::vector<unsigned char> encrypted(size);
    if (socket.recv_n(encrypted.data(), size) != static_cast<ssize_t>(size)) {
        return false;
    }
    
    auto decrypted = Crypto::decrypt(encrypted, key, iv);
    return message.deserialize(decrypted);
}
