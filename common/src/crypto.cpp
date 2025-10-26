// Add these methods to your existing common/src/crypto.cpp file

#include "../include/crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <stdexcept>

namespace RemoteAccessSystem {
namespace Common {

// If these methods don't exist, add them:

std::vector<uint8_t> Crypto::Encrypt(const std::vector<uint8_t>& plaintext,
                                       const std::vector<uint8_t>& key,
                                       const std::vector<uint8_t>& iv) const {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    std::vector<uint8_t> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int ciphertext_len = 0;

    try {
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
            throw std::runtime_error("Failed to initialize encryption");
        }

        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()) != 1) {
            throw std::runtime_error("Failed to encrypt data");
        }
        ciphertext_len = len;

        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
            throw std::runtime_error("Failed to finalize encryption");
        }
        ciphertext_len += len;

        ciphertext.resize(ciphertext_len);
        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }
}

std::vector<uint8_t> Crypto::Decrypt(const std::vector<uint8_t>& ciphertext,
                                       const std::vector<uint8_t>& key,
                                       const std::vector<uint8_t>& iv) const {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    std::vector<uint8_t> plaintext(ciphertext.size());
    int len = 0;
    int plaintext_len = 0;

    try {
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
            throw std::runtime_error("Failed to initialize decryption");
        }

        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()) != 1) {
            throw std::runtime_error("Failed to decrypt data");
        }
        plaintext_len = len;

        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
            throw std::runtime_error("Failed to finalize decryption");
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

std::vector<uint8_t> Crypto::GenerateKey() {
    std::vector<uint8_t> key(32); // 256 bits for AES-256
    if (RAND_bytes(key.data(), key.size()) != 1) {
        throw std::runtime_error("Failed to generate random key");
    }
    return key;
}

std::vector<uint8_t> Crypto::GenerateIV() {
    std::vector<uint8_t> iv(16); // 128 bits for AES block size
    if (RAND_bytes(iv.data(), iv.size()) != 1) {
        throw std::runtime_error("Failed to generate random IV");
    }
    return iv;
}

} // namespace Common
} // namespace RemoteAccessSystem