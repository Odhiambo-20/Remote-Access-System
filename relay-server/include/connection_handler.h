#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <ace/SOCK_Stream.h>
#include <ace/OS_NS_unistd.h>
#include <vector>
#include <tao/ORB.h>
#include <tao/Object.h>
#include "../../common/include/protocol.h"

namespace RemoteAccessSystem {
namespace RelayServer {

class ConnectionHandler {
public:
    ConnectionHandler(ACE_HANDLE client_socket, 
                     const std::vector<uint8_t>& key, 
                     const std::vector<uint8_t>& iv,
                     CORBA::ORB_ptr orb);
    ~ConnectionHandler();
    
    bool Handle();

private:
    bool ProcessMessage(const Common::Message& message, 
                       ACE_SOCK_Stream& sock_stream,
                       Common::Protocol& protocol);
    
    bool HandleConnectRequest(const Common::Message& message,
                             ACE_SOCK_Stream& sock_stream,
                             Common::Protocol& protocol);
    
    bool HandleDataTransfer(const Common::Message& message,
                           ACE_SOCK_Stream& sock_stream,
                           Common::Protocol& protocol);
    
    // FIXED: Removed duplicate parameter
    bool HandleDisconnectRequest(const Common::Message& message,
                                ACE_SOCK_Stream& sock_stream,
                                Common::Protocol& protocol);

    ACE_HANDLE client_socket_;
    std::vector<uint8_t> key_;
    std::vector<uint8_t> iv_;
    CORBA::ORB_var orb_;
};

} // namespace RelayServer
} // namespace RemoteAccessSystem

#endif // CONNECTION_HANDLER_H