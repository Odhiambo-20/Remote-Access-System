#ifndef REMOTE_ACCESS_SYSTEM_CRYPTO_H
#define REMOTE_ACCESS_SYSTEM_CRYPTO_H

#include <vector>
#include <string>

namespace RemoteAccessSystem {
namespace Common {

class Crypto {
public:
    Crypto();
    ~Crypto();

    // Generate encryption key (256-bit)
    static std::vector<unsigned char> GenerateKey();
    
    // Generate initialization vector (128-bit)
    static std::vector<unsigned char> GenerateIV();
    
    // Encrypt data using AES-256-CBC
    std::vector<unsigned char> Encrypt(
        const std::vector<unsigned char>& plaintext,
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& iv) const;
    
    // Decrypt data using AES-256-CBC
    std::vector<unsigned char> Decrypt(
        const std::vector<unsigned char>& ciphertext,
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& iv) const;

private:
    // Disable copy constructor and assignment operator
    Crypto(const Crypto&) = delete;
    Crypto& operator=(const Crypto&) = delete;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // REMOTE_ACCESS_SYSTEM_CRYPTO_H