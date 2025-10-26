#ifndef RELAY_CLIENT_H
#define RELAY_CLIENT_H

#include "protocol.h"
#include <ace/SOCK_Stream.h>
#include <string>

namespace RemoteAccessSystem {
namespace Common {

class RelayClient {
public:
    RelayClient(CORBA::ORB_ptr orb);
    ~RelayClient();

    // Connect to the relay server
    bool Connect(const std::string& relay_address);

    // Register PC with the relay server
    bool Register(const std::string& pc_id);

    // Handle incoming messages from the mobile app
    bool HandleMessage(const Message& message, Message& response);

private:
    Protocol protocol_;
    ACE_SOCK_Stream socket_;
    std::vector<unsigned char> key_;
    std::vector<unsigned char> iv_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // RELAY_CLIENT_H