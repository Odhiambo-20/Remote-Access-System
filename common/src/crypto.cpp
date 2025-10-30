#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <stdexcept>
#include <cstring>

namespace RemoteAccessSystem {
namespace Common {

Crypto::Crypto() {
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}

Crypto::~Crypto() {
    // Cleanup OpenSSL
    EVP_cleanup();
    ERR_free_strings();
}

std::vector<unsigned char> Crypto::GenerateKey() {
    std::vector<unsigned char> key(32); // 256 bits
    if (RAND_bytes(key.data(), key.size()) != 1) {
        throw std::runtime_error("Failed to generate encryption key");
    }
    return key;
}

std::vector<unsigned char> Crypto::GenerateIV() {
    std::vector<unsigned char> iv(16); // 128 bits
    if (RAND_bytes(iv.data(), iv.size()) != 1) {
        throw std::runtime_error("Failed to generate initialization vector");
    }
    return iv;
}

std::vector<unsigned char> Crypto::Encrypt(
    const std::vector<unsigned char>& plaintext,
    const std::vector<unsigned char>& key,
    const std::vector<unsigned char>& iv) const {
    
    if (key.size() != 32) {
        throw std::invalid_argument("Key must be 256 bits (32 bytes)");
    }
    if (iv.size() != 16) {
        throw std::invalid_argument("IV must be 128 bits (16 bytes)");
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    // Initialize encryption
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }
    
    // Allocate output buffer (plaintext size + block size for padding)
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0;
    int ciphertext_len = 0;
    
    // Encrypt
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption failed");
    }
    ciphertext_len = len;
    
    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption finalization failed");
    }
    ciphertext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    // Resize to actual size
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::vector<unsigned char> Crypto::Decrypt(
    const std::vector<unsigned char>& ciphertext,
    const std::vector<unsigned char>& key,
    const std::vector<unsigned char>& iv) const {
    
    if (key.size() != 32) {
        throw std::invalid_argument("Key must be 256 bits (32 bytes)");
    }
    if (iv.size() != 16) {
        throw std::invalid_argument("IV must be 128 bits (16 bytes)");
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    // Initialize decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }
    
    // Allocate output buffer
    std::vector<unsigned char> plaintext(ciphertext.size());
    int len = 0;
    int plaintext_len = 0;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption failed");
    }
    plaintext_len = len;
    
    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption finalization failed");
    }
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    // Resize to actual size
    plaintext.resize(plaintext_len);
    return plaintext;
}

} // namespace Common
} // namespace RemoteAccessSystem