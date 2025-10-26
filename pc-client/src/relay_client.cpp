#include <tao/ORB.h>
#include "relay_client.h"
#include "utils.h"
#include "file_manager.h"

namespace RemoteAccessSystem {
namespace Common {

RelayClient::RelayClient(CORBA::ORB_ptr orb) : protocol_(orb) {
    key_ = Crypto::GenerateKey();
    iv_ = Crypto::GenerateIV();
}

RelayClient::~RelayClient() {
    socket_.close();
}

bool RelayClient::Connect(const std::string& relay_address) {
    ACE_INET_Addr addr(relay_address.c_str();
    return protocol_.Connect(addr, socket_);
}

bool RelayClient::Register(const std::string& pc_id) {
    Message msg;
    msg.header.type = MessageType::BIND_REQUEST;
    msg.header.message_id = Utils::GetTimestamp();
    msg.header.timestamp = Utils::GetTimestamp();
    msg.payload = Utils::StringToBytes(pc_id);

    return protocol_.SendMessage(socket_, msg, key_, iv_);
}

bool RelayClient::HandleMessage(const Message& message, Message& response) {
    FileManager fm;
    response.header.message_id = Utils::GetTimestamp();
    response.header.timestamp = Utils::GetTimestamp();

    switch (message.header.type) {
        case MessageType::FILE_LIST_REQUEST: {
            auto path = Utils::BytesToString(message.payload);
            auto files = fm.ListDirectory(path);
            response.header.type = MessageType::FILE_LIST_RESPONSE;
            response.payload = Utils::StringToBytes(std::accumulate(files.begin(), files.end(), std::string(),
                [](const std::string& a, const std::string& b) { return a + b + "\n"; });
            break;
        }
        case MessageType::FILE_DOWNLOAD: {
            auto path = Utils::BytesToString(message.payload);
            response.header.type = MessageType::FILE_DOWNLOAD;
            response.payload = fm.ReadFile(path);
            break;
        }
        case MessageType::FILE_UPLOAD: {
            auto parts = Utils::BytesToString(message.payload).split(':');
            if (parts.size() == 2) {
                fm.WriteFile(parts[0], Utils::StringToBytes(parts[1]);
                response.header.type = MessageType::FILE_UPLOAD;
                response.payload = Utils::StringToBytes("SUCCESS");
            }
            break;
        }
        case MessageType::SHARE_LINK_REQUEST: {
            auto path = Utils::BytesToString(message.payload);
            response.header.type = MessageType::SHARE_LINK_RESPONSE;
            response.payload = Utils::StringToBytes(fm.GenerateShareLink(path);
            break;
        }
        default:
            return false;
    }
    return true;
}

} // namespace Common
} // namespace RemoteAccessSystem