#include "connection_handler.h"
#include <ace/Log_Msg.h>
#include <ace/SOCK_Stream.h>
#include <ace/OS_NS_unistd.h>
#include "../../common/include/protocol.h"

namespace RemoteAccessSystem {
namespace RelayServer {

ConnectionHandler::ConnectionHandler(ACE_HANDLE client_socket, 
                                     const std::vector<uint8_t>& key, 
                                     const std::vector<uint8_t>& iv,
                                     CORBA::ORB_ptr orb)
    : client_socket_(client_socket), key_(key), iv_(iv), orb_(CORBA::ORB::_duplicate(orb)) {
}

ConnectionHandler::~ConnectionHandler() {
    if (client_socket_ != ACE_INVALID_HANDLE) {
        ACE_OS::closesocket(client_socket_);
    }
}

bool ConnectionHandler::Handle() {
    Common::Message message;
    
    if (CORBA::is_nil(orb_.in())) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) ORB is nil\n")));
        return false;
    }
    
    // FIXED: Protocol constructor doesn't take ORB parameter
    Common::Protocol protocol;

    // Convert ACE_HANDLE to ACE_SOCK_Stream
    ACE_SOCK_Stream sock_stream;
    sock_stream.set_handle(client_socket_);

    if (!protocol.ReceiveMessage(sock_stream, message, key_, iv_)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Failed to receive message from client\n")));
        return false;
    }

    // Process message based on type
    bool result = ProcessMessage(message, sock_stream, protocol);
    
    return result;
}

bool ConnectionHandler::ProcessMessage(const Common::Message& message, 
                                       ACE_SOCK_Stream& sock_stream,
                                       Common::Protocol& protocol) {
    // FIXED: Use message.header.type instead of message.type
    // FIXED: Use only AUTH_REQUEST, AUTH_RESPONSE, BIND_REQUEST, BIND_RESPONSE
    switch (message.header.type) {
        case Common::MessageType::AUTH_REQUEST:
            ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) Processing AUTH_REQUEST\n")));
            return HandleConnectRequest(message, sock_stream, protocol);
            
        case Common::MessageType::BIND_REQUEST:
            ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) Processing BIND_REQUEST\n")));
            return HandleDataTransfer(message, sock_stream, protocol);
            
        case Common::MessageType::AUTH_RESPONSE:
            ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) Processing AUTH_RESPONSE\n")));
            return HandleDisconnectRequest(message, sock_stream, protocol);
            
        default:
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Unknown message type: %d\n"), 
                      static_cast<int>(message.header.type)));
            return false;
    }
}

bool ConnectionHandler::HandleConnectRequest(const Common::Message& message,
                                             ACE_SOCK_Stream& sock_stream,
                                             Common::Protocol& protocol) {
    // TODO: Implement connection request handling
    Common::Message response;
    response.header.type = Common::MessageType::AUTH_RESPONSE;  // FIXED
    response.header.message_id = message.header.message_id;
    response.header.timestamp = message.header.timestamp;
    response.payload = std::vector<uint8_t>{'O', 'K'};
    
    if (!protocol.SendMessage(sock_stream, response, key_, iv_)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Failed to send connect response\n")));
        return false;
    }
    
    return true;
}

bool ConnectionHandler::HandleDataTransfer(const Common::Message& message,
                                           ACE_SOCK_Stream& sock_stream,
                                           Common::Protocol& protocol) {
    // TODO: Implement data transfer handling
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Received %d bytes of data\n"), 
              message.payload.size()));
    return true;
}

bool ConnectionHandler::HandleDisconnectRequest(const Common::Message& message,
                                                ACE_SOCK_Stream& sock_stream,
                                                Common::Protocol& protocol) {
    // TODO: Implement disconnect handling
    Common::Message response;
    response.header.type = Common::MessageType::BIND_RESPONSE;  // FIXED
    response.header.message_id = message.header.message_id;
    response.header.timestamp = message.header.timestamp;
    
    if (!protocol.SendMessage(sock_stream, response, key_, iv_)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) Failed to send disconnect response\n")));
        return false;
    }
    
    return true;
}

} // namespace RelayServer
} // namespace RemoteAccessSystem