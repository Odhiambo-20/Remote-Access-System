#ifndef MESSAGE_ROUTER_H
#define MESSAGE_ROUTER_H

#include "message.h"
#include <map>
#include <string>

namespace RemoteAccessSystem {
namespace Common {

class MessageRouter {
public:
    MessageRouter() = default;
    ~MessageRouter() = default;

    // Register a client (PC or mobile) with its ID
    void RegisterClient(const std::string& client_id, ACE_SOCK_Stream& socket);

    // Route a message to the destination client
    bool RouteMessage(const Message& message, const std::string& dest_id);

private:
    std::map<std::string, ACE_SOCK_Stream> clients_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // MESSAGE_ROUTER_H