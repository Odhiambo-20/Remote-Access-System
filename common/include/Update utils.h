#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>
#include <ace/Log_Priority.h>

namespace RemoteAccessSystem {
namespace Common {

class Utils {
public:
    // Timestamp generation
    static uint64_t GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
    
    // String/Bytes conversion
    static std::vector<unsigned char> StringToBytes(const std::string& str) {
        return std::vector<unsigned char>(str.begin(), str.end());
    }
    
    static std::string BytesToString(const std::vector<unsigned char>& bytes) {
        return std::string(bytes.begin(), bytes.end());
    }
    
    // Random token generation
    static std::string GenerateRandomToken(size_t length);
    
    // Logging
    static void Log(ACE_Log_Priority priority, const std::string& message);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // UTILS_H