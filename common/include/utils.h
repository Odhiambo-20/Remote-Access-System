#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

class Utils {
public:
    static bool ReadConfig(const std::string& config_file, std::string& content);
    static void Log(ACE_Log_Priority priority, const std::string& message);
     static std::string GetTimestampString();
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // UTILS_H
