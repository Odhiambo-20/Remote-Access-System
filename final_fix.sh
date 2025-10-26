#!/bin/bash

echo "=== Final Comprehensive Fix ==="

# 1. Fix utils.cpp - missing closing parentheses
cat > common/src/utils.cpp << 'EOF'
#include "utils.h"
#include <ace/Log_Msg.h>
#include <algorithm>
#include <cctype>

namespace RemoteAccessSystem {
namespace Common {

std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Utils::BytesToString(const std::vector<uint8_t>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

std::vector<uint8_t> Utils::StringToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

uint64_t Utils::GetTimestamp() {
    return static_cast<uint64_t>(ACE_OS::gettimeofday().sec());
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# 2. Fix protocol.cpp - add missing ACE includes
cat > common/src/protocol.cpp << 'EOF'
#include "protocol.h"
#include <ace/Log_Msg.h>
#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <cstring>

namespace RemoteAccessSystem {
namespace Common {

std::vector<uint8_t> serializeMessage(MessageType type, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> message;
    message.reserve(7 + payload.size());
    
    message.push_back((PROTOCOL_VERSION >> 8) & 0xFF);
    message.push_back(PROTOCOL_VERSION & 0xFF);
    message.push_back(static_cast<uint8_t>(type));
    
    uint32_t length = payload.size();
    message.push_back((length >> 24) & 0xFF);
    message.push_back((length >> 16) & 0xFF);
    message.push_back((length >> 8) & 0xFF);
    message.push_back(length & 0xFF);
    
    message.insert(message.end(), payload.begin(), payload.end());
    return message;
}

bool deserializeMessage(const std::vector<uint8_t>& data, MessageType& type, 
                       std::vector<uint8_t>& payload) {
    if (data.size() < 7) return false;
    
    uint16_t version = (data[0] << 8) | data[1];
    if (version != PROTOCOL_VERSION) return false;
    
    type = static_cast<MessageType>(data[2]);
    
    uint32_t length = (static_cast<uint32_t>(data[3]) << 24) |
                     (static_cast<uint32_t>(data[4]) << 16) |
                     (static_cast<uint32_t>(data[5]) << 8) |
                      static_cast<uint32_t>(data[6]);
    
    if (data.size() < 7 + length) return false;
    
    payload.assign(data.begin() + 7, data.begin() + 7 + length);
    return true;
}

Protocol::Protocol(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {}

bool Protocol::Connect(const ACE_INET_Addr& addr, ACE_SOCK_Stream& stream) {
    ACE_SOCK_Connector connector;
    if (connector.connect(stream, addr) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to connect to %s:%d\n"),
                   addr.get_host_name(), addr.get_port_number()));
        return false;
    }
    return true;
}

bool Protocol::SendMessage(ACE_SOCK_Stream& stream, const Message& msg,
                          const std::vector<uint8_t>& key,
                          const std::vector<uint8_t>& iv) {
    std::vector<uint8_t> payload = msg.payload;
    
    if (!key.empty() && !iv.empty()) {
        payload = Crypto::Encrypt(payload, key, iv);
    }
    
    std::vector<uint8_t> data = serializeMessage(msg.header.type, payload);
    
    ssize_t bytes_sent = stream.send_n(data.data(), data.size());
    if (bytes_sent != static_cast<ssize_t>(data.size())) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to send message\n")));
        return false;
    }
    
    return true;
}

bool Protocol::ReceiveMessage(ACE_SOCK_Stream& stream, Message& msg,
                             const std::vector<uint8_t>& key,
                             const std::vector<uint8_t>& iv) {
    uint8_t header[7];
    ssize_t bytes_received = stream.recv_n(header, 7);
    
    if (bytes_received != 7) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to receive message header\n")));
        return false;
    }
    
    uint32_t length = (static_cast<uint32_t>(header[3]) << 24) |
                     (static_cast<uint32_t>(header[4]) << 16) |
                     (static_cast<uint32_t>(header[5]) << 8) |
                      static_cast<uint32_t>(header[6]);
    
    std::vector<uint8_t> payload(length);
    bytes_received = stream.recv_n(payload.data(), length);
    
    if (bytes_received != static_cast<ssize_t>(length)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to receive message payload\n")));
        return false;
    }
    
    msg.header.type = static_cast<MessageType>(header[2]);
    
    if (!key.empty() && !iv.empty()) {
        msg.payload = Crypto::Decrypt(payload, key, iv);
    } else {
        msg.payload = payload;
    }
    
    return true;
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# 3. Fix crypto header to include KEY_SIZE and IV_SIZE constants
cat > common/include/crypto.h << 'EOF'
#pragma once

#include <vector>
#include <string>
#include <openssl/evp.h>

namespace RemoteAccessSystem {
namespace Common {

class Crypto {
public:
    static constexpr size_t KEY_SIZE = 32;  // 256 bits for AES-256
    static constexpr size_t IV_SIZE = 16;   // 128 bits for AES block size
    
    Crypto();
    ~Crypto();
    
    static std::vector<uint8_t> GenerateKey();
    static std::vector<uint8_t> GenerateIV();
    
    static std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& plaintext,
                                       const std::vector<uint8_t>& key,
                                       const std::vector<uint8_t>& iv);
    
    static std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& ciphertext,
                                       const std::vector<uint8_t>& key,
                                       const std::vector<uint8_t>& iv);
    
    static std::string GenerateRandomString(size_t length);
    
private:
    EVP_CIPHER_CTX* encrypt_ctx_;
    EVP_CIPHER_CTX* decrypt_ctx_;
};

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# 4. Fix protocol.h to include ACE headers
cat > common/include/protocol.h << 'EOF'
#pragma once

#include <vector>
#include <cstdint>
#include <tao/ORB.h>
#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
#include "message.h"
#include "crypto.h"

namespace RemoteAccessSystem {
namespace Common {

constexpr uint16_t PROTOCOL_VERSION = 0x0001;

std::vector<uint8_t> serializeMessage(MessageType type, const std::vector<uint8_t>& payload);
bool deserializeMessage(const std::vector<uint8_t>& data, MessageType& type, 
                       std::vector<uint8_t>& payload);

class Protocol {
public:
    explicit Protocol(CORBA::ORB_ptr orb);
    
    bool Connect(const ACE_INET_Addr& addr, ACE_SOCK_Stream& stream);
    
    bool SendMessage(ACE_SOCK_Stream& stream, const Message& msg,
                    const std::vector<uint8_t>& key,
                    const std::vector<uint8_t>& iv);
    
    bool ReceiveMessage(ACE_SOCK_Stream& stream, Message& msg,
                       const std::vector<uint8_t>& key,
                       const std::vector<uint8_t>& iv);
    
private:
    CORBA::ORB_var orb_;
};

} // namespace Common
} // namespace RemoteAccessSystem
EOF

echo "✅ All fixes applied!"
