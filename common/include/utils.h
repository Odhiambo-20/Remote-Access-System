#ifndef REMOTE_ACCESS_SYSTEM_UTILS_H
#define REMOTE_ACCESS_SYSTEM_UTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include <ace/Log_Priority.h>

namespace RemoteAccessSystem {
namespace Common {

class Utils {
public:
    // Get current timestamp in milliseconds
    static uint64_t GetTimestamp();
    
    // Convert string to byte vector
    static std::vector<unsigned char> StringToBytes(const std::string& str);
    
    // Convert byte vector to string
    static std::string BytesToString(const std::vector<unsigned char>& bytes);
    
    // Generate a random token of specified length
    static std::string GenerateRandomToken(size_t length);
    
    // Logging utility function
    static void Log(ACE_Log_Priority priority, const std::string& message);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // REMOTE_ACCESS_SYSTEM_UTILS_H