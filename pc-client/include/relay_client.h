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
    HEARTBEAT = 6
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
    
    // Serialization helpers
    std::vector<unsigned char> SerializeMessage(const Message& msg);
    Message DeserializeMessage(const std::vector<unsigned char>& data);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // RELAY_CLIENT_H