#include <tao/ORB.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>
#include <ace/Log_Msg.h>
#include "protocol.h"
#include "message.h"
#include "crypto.h"
#include "utils.h"
#include "message_router.h"
#include "utils.h"

namespace RemoteAccessSystem {
namespace Common {

void MessageRouter::RegisterClient(const std::string& client_id, ACE_SOCK_Stream& socket) {
    clients_[client_id] = socket;
    ACE_DEBUG((LM_INFO, ACE_TEXT( "Registered client: " + client_id);
}

bool MessageRouter::RouteMessage(const Message& message, const std::string& dest_id) {
    auto it = clients_.find(dest_id);
    if (it == clients_.end()) {
        ACE_ERROR((LM_ERROR, ACE_TEXT( "Destination client not found: " + dest_id);
        return false;
    }

    Protocol protocol(nullptr);
    auto key = Crypto::GenerateKey();
    auto iv = Crypto::GenerateIV();
    return protocol.SendMessage(it->second, message, key, iv);
}

} // namespace Common
} // namespace RemoteAccessSystem