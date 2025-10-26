#!/bin/bash

echo "Fixing extra parentheses..."

# Fix all .cpp files that have extra parentheses from the bad sed command
find . -name "*.cpp" -type f | while read file; do
    # Remove triple parentheses at end of lines (but keep proper ones)
    sed -i 's/)));$/);/g' "$file"
    
    # Fix cases where we have )); at end but shouldn't
    # Only for non-ACE_ERROR/ACE_DEBUG lines
    sed -i '/ACE_ERROR\|ACE_DEBUG/!s/));$/);/g' "$file"
done

# Specifically fix protocol.cpp
cat > common/src/protocol.cpp << 'EOF'
#include "protocol.h"
#include <ace/Log_Msg.h>
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

# Fix crypto.cpp
cat > common/src/crypto.cpp << 'EOF'
#include "crypto.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdexcept>
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

Crypto::Crypto() {
    encrypt_ctx_ = EVP_CIPHER_CTX_new();
    decrypt_ctx_ = EVP_CIPHER_CTX_new();
    
    if (!encrypt_ctx_ || !decrypt_ctx_) {
        throw std::runtime_error("Failed to create cipher contexts");
    }
}

Crypto::~Crypto() {
    if (encrypt_ctx_) {
        EVP_CIPHER_CTX_free(encrypt_ctx_);
    }
    if (decrypt_ctx_) {
        EVP_CIPHER_CTX_free(decrypt_ctx_);
    }
}

std::vector<uint8_t> Crypto::GenerateKey() {
    std::vector<uint8_t> key(KEY_SIZE);
    if (RAND_bytes(key.data(), KEY_SIZE) != 1) {
        throw std::runtime_error("Failed to generate random key");
    }
    return key;
}

std::vector<uint8_t> Crypto::GenerateIV() {
    std::vector<uint8_t> iv(IV_SIZE);
    if (RAND_bytes(iv.data(), IV_SIZE) != 1) {
        throw std::runtime_error("Failed to generate random IV");
    }
    return iv;
}

std::vector<uint8_t> Crypto::Encrypt(const std::vector<uint8_t>& plaintext,
                                     const std::vector<uint8_t>& key,
                                     const std::vector<uint8_t>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, 
                          key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }
    
    std::vector<uint8_t> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0;
    int ciphertext_len = 0;
    
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                         plaintext.data(), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption failed");
    }
    ciphertext_len = len;
    
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption finalization failed");
    }
    ciphertext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::vector<uint8_t> Crypto::Decrypt(const std::vector<uint8_t>& ciphertext,
                                     const std::vector<uint8_t>& key,
                                     const std::vector<uint8_t>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, 
                          key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }
    
    std::vector<uint8_t> plaintext(ciphertext.size());
    int len = 0;
    int plaintext_len = 0;
    
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext.data(), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption failed");
    }
    plaintext_len = len;
    
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption finalization failed");
    }
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    return plaintext;
}

std::string Crypto::GenerateRandomString(size_t length) {
    std::vector<uint8_t> random_bytes(length);
    if (RAND_bytes(random_bytes.data(), length) != 1) {
        throw std::runtime_error("Failed to generate random string");
    }
    
    static const char charset[] = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[random_bytes[i] % (sizeof(charset) - 1)];
    }
    
    return result;
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

echo "✅ Parentheses fixed!"
