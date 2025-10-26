#ifndef CRYPTO_H
#define CRYPTO_H

#include <vector>
#include <string>
#include <cstdint>

namespace RemoteAccessSystem {
namespace Common {

class Crypto {
public:
    // Generate a random 256-bit AES key
    static std::vector<uint8_t> GenerateKey();
    
    // Generate a random 128-bit IV
    static std::vector<uint8_t> GenerateIV();
    
    // Encrypt data using AES-256-CBC
    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& plaintext,
                                 const std::vector<uint8_t>& key,
                                 const std::vector<uint8_t>& iv) const;
    
    // Decrypt data using AES-256-CBC
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& ciphertext,
                                 const std::vector<uint8_t>& key,
                                 const std::vector<uint8_t>& iv) const;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // CRYPTO_H