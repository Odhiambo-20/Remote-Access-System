#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace RemoteAccessSystem {
namespace Common {

enum class MessageType : uint8_t {
    UNKNOWN = 0,
    AUTH_REQUEST = 1,
    AUTH_RESPONSE = 2,
    PC_LIST_REQUEST = 3,
    PC_LIST_RESPONSE = 4,
    CONNECT_REQUEST = 5,
    CONNECT_RESPONSE = 6,
    DISCONNECT = 7,
    HEARTBEAT = 8,
    FILE_REQUEST = 9,
    FILE_RESPONSE = 10,
    COMMAND = 11,
    COMMAND_RESPONSE = 12
};

struct ProtocolHeader {
    uint32_t magic;
    uint32_t version;
    MessageType type;
    uint64_t message_id;
    uint64_t timestamp;
    uint32_t payload_size;
    uint32_t checksum;
};

class Message {
public:
    Message();
    explicit Message(MessageType type);
    Message(MessageType type, const std::string& payload);
    ~Message();

    void setType(MessageType type);
    MessageType getType() const;
    void setPayload(const std::string& payload);
    std::string getPayload() const;
    
    std::vector<unsigned char> serialize() const;
    bool deserialize(const std::vector<unsigned char>& data);
    
    ProtocolHeader& getHeader();
    const ProtocolHeader& getHeader() const;

private:
    MessageType type_;
    std::string payload_;
    ProtocolHeader header_;
};

} // namespace Common
} // namespace RemoteAccessSystem
