#include "../include/utils.h"
#include <ace/Log_Msg.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace RemoteAccessSystem {
namespace Common {

bool Utils::ReadConfig(const std::string& config_file, std::string& content) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to open config file: " + config_file);
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    file.close();
    return true;
}

void Utils::Log(ACE_Log_Priority priority, const std::string& message) {
    ACE_LOG_MSG->log(priority, "%s", message.c_str());
}

std::string Utils::GetTimestampString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace Common
} // namespace RemoteAccessSystem