#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>

namespace RemoteAccessSystem {
namespace Common {

class Crypto {
public:
    static std::vector<unsigned char> GenerateKey() {
        std::vector<unsigned char> key(32);
        RAND_bytes(key.data(), 32);
        return key;
    }
    
    static std::vector<unsigned char> GenerateIV() {
        std::vector<unsigned char> iv(16);
        RAND_bytes(iv.data(), 16);
        return iv;
    }
    
    static std::vector<unsigned char> encrypt(
        const std::vector<unsigned char>& plaintext,
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& iv);
    
    static std::vector<unsigned char> decrypt(
        const std::vector<unsigned char>& ciphertext,
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& iv);
};

} // namespace Common
} // namespace RemoteAccessSystem
