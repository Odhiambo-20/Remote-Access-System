#!/bin/bash
echo "Backing up original files..."
cp common/include/protocol.h common/include/protocol.h.backup 2>/dev/null
cp common/src/protocol.cpp common/src/protocol.cpp.backup 2>/dev/null
cp common/src/crypto.cpp common/src/crypto.cpp.backup 2>/dev/null

echo "Fixing protocol.h..."
cat > common/include/protocol.h << 'EOF'
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include <cstdint>

namespace RemoteAccess {

const uint16_t PROTOCOL_VERSION = 1;
const uint16_t DEFAULT_PORT = 8888;

enum class MessageType : uint8_t {
    CONNECT_REQUEST = 1,
    CONNECT_RESPONSE = 2,
    AUTHENTICATE = 3,
    AUTH_RESPONSE = 4,
    DATA = 5,
    PING = 6,
    PONG = 7,
    DISCONNECT = 8,
    ERROR = 9
};

std::vector<uint8_t> serializeMessage(MessageType type, const std::vector<uint8_t>& payload);
bool deserializeMessage(const std::vector<uint8_t>& data, MessageType& type, std::vector<uint8_t>& payload);

}

#endif
EOF

echo "Fixing protocol.cpp..."
cat > common/src/protocol.cpp << 'EOF'
#include "protocol.h"

namespace RemoteAccess {

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

bool deserializeMessage(const std::vector<uint8_t>& data, MessageType& type, std::vector<uint8_t>& payload) {
    if (data.size() < 7) return false;
    
    uint16_t version = (static_cast<uint16_t>(data[0]) << 8) | data[1];
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

}
EOF

echo "Fixing crypto.cpp..."
cat > common/src/crypto.cpp << 'EOF'
#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdexcept>

namespace RemoteAccess {

const size_t Crypto::KEY_LENGTH = 32;
const size_t Crypto::IV_LENGTH = 12;
const size_t Crypto::TAG_LENGTH = 16;

std::string Crypto::generateSalt(size_t length) {
    std::vector<uint8_t> salt(length);
    if (RAND_bytes(salt.data(), length) != 1) {
        throw std::runtime_error("Failed to generate random salt");
    }
    return base64Encode(salt);
}

std::string Crypto::generateToken(size_t length) {
    std::vector<uint8_t> token(length);
    if (RAND_bytes(token.data(), length) != 1) {
        throw std::runtime_error("Failed to generate random token");
    }
    
    std::string result;
    result.reserve(length * 2);
    const char* hex = "0123456789abcdef";
    for (uint8_t byte : token) {
        result += hex[(byte >> 4) & 0xF];
        result += hex[byte & 0xF];
    }
    return result;
}

std::string Crypto::hashPassword(const std::string& password, const std::string& salt) {
    std::vector<uint8_t> salt_bytes = base64Decode(salt);
    std::vector<uint8_t> hash(32);
    
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                          salt_bytes.data(), salt_bytes.size(),
                          100000, EVP_sha256(),
                          hash.size(), hash.data()) != 1) {
        throw std::runtime_error("Password hashing failed");
    }
    return base64Encode(hash);
}

bool Crypto::verifyPassword(const std::string& password, const std::string& salt, const std::string& hash) {
    return hashPassword(password, salt) == hash;
}

std::vector<uint8_t> Crypto::encrypt(const std::vector<uint8_t>& plaintext, const std::string& key) {
    std::vector<uint8_t> iv(IV_LENGTH);
    if (RAND_bytes(iv.data(), IV_LENGTH) != 1) {
        throw std::runtime_error("Failed to generate IV");
    }
    
    std::vector<uint8_t> key_bytes(KEY_LENGTH);
    SHA256(reinterpret_cast<const unsigned char*>(key.c_str()), key.length(), key_bytes.data());
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create cipher context");
    
    try {
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key_bytes.data(), iv.data()) != 1) {
            throw std::runtime_error("Encryption initialization failed");
        }
        
        std::vector<uint8_t> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_gcm()));
        int len;
        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()) != 1) {
            throw std::runtime_error("Encryption failed");
        }
        int ciphertext_len = len;
        
        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
            throw std::runtime_error("Encryption finalization failed");
        }
        ciphertext_len += len;
        ciphertext.resize(ciphertext_len);
        
        std::vector<uint8_t> tag(TAG_LENGTH);
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LENGTH, tag.data()) != 1) {
            throw std::runtime_error("Failed to get authentication tag");
        }
        
        std::vector<uint8_t> result;
        result.reserve(IV_LENGTH + ciphertext.size() + TAG_LENGTH);
        result.insert(result.end(), iv.begin(), iv.end());
        result.insert(result.end(), ciphertext.begin(), ciphertext.end());
        result.insert(result.end(), tag.begin(), tag.end());
        
        EVP_CIPHER_CTX_free(ctx);
        return result;
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }
}

std::vector<uint8_t> Crypto::decrypt(const std::vector<uint8_t>& ciphertext, const std::string& key) {
    if (ciphertext.size() < IV_LENGTH + TAG_LENGTH) {
        throw std::runtime_error("Ciphertext too short");
    }
    
    std::vector<uint8_t> iv(ciphertext.begin(), ciphertext.begin() + IV_LENGTH);
    std::vector<uint8_t> tag(ciphertext.end() - TAG_LENGTH, ciphertext.end());
    std::vector<uint8_t> encrypted_data(ciphertext.begin() + IV_LENGTH, ciphertext.end() - TAG_LENGTH);
    
    std::vector<uint8_t> key_bytes(KEY_LENGTH);
    SHA256(reinterpret_cast<const unsigned char*>(key.c_str()), key.length(), key_bytes.data());
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create cipher context");
    
    try {
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key_bytes.data(), iv.data()) != 1) {
            throw std::runtime_error("Decryption initialization failed");
        }
        
        std::vector<uint8_t> plaintext(encrypted_data.size());
        int len;
        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, encrypted_data.data(), encrypted_data.size()) != 1) {
            throw std::runtime_error("Decryption failed");
        }
        int plaintext_len = len;
        
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LENGTH, tag.data()) != 1) {
            throw std::runtime_error("Failed to set authentication tag");
        }
        
        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
            throw std::runtime_error("Decryption finalization failed");
        }
        plaintext_len += len;
        plaintext.resize(plaintext_len);
        
        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }
}

std::string Crypto::base64Encode(const std::vector<uint8_t>& data) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    
    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);
    
    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);
    
    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return result;
}

std::string Crypto::base64Encode(const std::string& data) {
    return base64Encode(std::vector<uint8_t>(data.begin(), data.end()));
}

std::vector<uint8_t> Crypto::base64Decode(const std::string& encoded) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_new_mem_buf(encoded.data(), encoded.length());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    
    std::vector<uint8_t> result(encoded.length());
    int decoded_length = BIO_read(bio, result.data(), encoded.length());
    BIO_free_all(bio);
    
    if (decoded_length < 0) {
        throw std::runtime_error("Base64 decoding failed");
    }
    
    result.resize(decoded_length);
    return result;
}

}
EOF

echo "All files fixed!"
echo "Cleaning build directory..."
rm -rf build
mkdir build

echo "Done! Now run: ./build.sh"
