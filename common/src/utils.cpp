#include "utils.h"
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

uint64_t Utils::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

std::vector<unsigned char> Utils::StringToBytes(const std::string& str) {
    return std::vector<unsigned char>(str.begin(), str.end());
}

std::string Utils::BytesToString(const std::vector<unsigned char>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

std::string Utils::GenerateRandomToken(size_t length) {
    const char charset[] = 
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = sizeof(charset) - 2;
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, max_index);
    
    std::string token;
    token.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        token += charset[distribution(generator)];
    }
    
    return token;
}

void Utils::Log(ACE_Log_Priority priority, const std::string& message) {
    ACE_Log_Msg::instance()->log(priority, ACE_TEXT("%s\n"), message.c_str());
}

} // namespace Common
} // namespace 