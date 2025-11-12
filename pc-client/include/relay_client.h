#ifndef RELAY_CLIENT_H
#define RELAY_CLIENT_H

#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include "../../common/include/crypto.h"
#include <string>
#include <vector>
#include <cstdint>

namespace RemoteAccessSystem {
namespace Common {

enum class MessageType : uint8_t {
    AUTH_REQUEST = 0,
    AUTH_RESPONSE = 1,
    FILE_LIST_REQUEST = 2,
    FILE_LIST_RESPONSE = 3,
    FILE_DOWNLOAD = 4,
    FILE_UPLOAD = 5,
    HEARTBEAT = 6,
    FILE_COPY = 7,
    FILE_MOVE = 8,
    FILE_RENAME = 9,
    FILE_DELETE = 10,
    CREATE_FOLDER = 11
};

struct Message {
    MessageType type;
    uint64_t message_id;
    uint64_t timestamp;
    std::vector<unsigned char> payload;
};

class RelayClient {
public:
    RelayClient();
    ~RelayClient();

    // Connect to the relay server
    bool Connect(const std::string& relay_address);

    // Register PC with the relay server
    bool Register(const std::string& pc_id);

    // Handle incoming messages from the mobile app
    bool HandleMessage(const Message& message, Message& response);
    
    // Send and receive messages
    bool SendMessage(const Message& msg);
    bool ReceiveMessage(Message& msg);

private:
    ACE_SOCK_Stream socket_;
    Crypto crypto_;
    std::vector<unsigned char> key_;
    std::vector<unsigned char> iv_;
    
    // File operation handlers
    bool HandleDownload(const std::string& file_path, Message& response);
    bool HandleCopy(const std::string& source_path, const std::string& dest_path, Message& response);
    bool HandleMove(const std::string& source_path, const std::string& dest_path, Message& response);
    bool HandleRename(const std::string& old_path, const std::string& new_path, Message& response);
    bool HandleDelete(const std::string& file_path, Message& response);
    bool HandleCreateFolder(const std::string& folder_path, Message& response);
    
    // Serialization helpers
    std::vector<unsigned char> SerializeMessage(const Message& msg);
    Message DeserializeMessage(const std::vector<unsigned char>& data);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // RELAY_CLIENT_H