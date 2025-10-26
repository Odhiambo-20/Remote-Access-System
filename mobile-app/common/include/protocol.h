#pragma once
#include "message.h"
#include <vector>
#include <ace/SOCK_Stream.h>

namespace RemoteAccessSystem {
namespace Common {

class Protocol {
public:
    Protocol();
    ~Protocol() = default;
    
    static bool SendMessage(ACE_SOCK_Stream& socket, const Message& message,
                          const std::vector<unsigned char>& key,
                          const std::vector<unsigned char>& iv);
    
    static bool ReceiveMessage(ACE_SOCK_Stream& socket, Message& message,
                             const std::vector<unsigned char>& key,
                             const std::vector<unsigned char>& iv);

private:
    static uint32_t calculateChecksum(const std::vector<unsigned char>& data);
};

} // namespace Common
} // namespace RemoteAccessSystem
