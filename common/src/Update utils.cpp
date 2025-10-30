#include "utils.h"
#include <ace/Log_Msg.h>
#include <random>
#include <sstream>
#include <iomanip>

namespace RemoteAccessSystem {
namespace Common {

std::string Utils::GenerateRandomToken(size_t length) {
    static const char charset[] = 
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    
    std::string token;
    token.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        token += charset[dis(gen)];
    }
    
    return token;
}

void Utils::Log(ACE_Log_Priority priority, const std::string& message) {
    ACE_DEBUG((priority, ACE_TEXT("%s\n"), message.c_str()));
}

} // namespace Common
} // namespace RemoteAccessSystem