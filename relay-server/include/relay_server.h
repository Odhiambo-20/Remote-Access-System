#ifndef RELAY_SERVER_H
#define RELAY_SERVER_H

#include "message_router.h"
#include <ace/SOCK_Acceptor.h>
#include <tao/ORB.h>

namespace RemoteAccessSystem {
namespace Common {

class RelayServer {
public:
    RelayServer(CORBA::ORB_ptr orb);
    ~RelayServer();

    // Start the relay server
    bool Start(const std::string& address, uint16_t port);

    // Stop the relay server
    void Stop();

private:
    CORBA::ORB_var orb_;
    ACE_SOCK_Acceptor acceptor_;
    MessageRouter router_;
    bool running_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // RELAY_SERVER_H