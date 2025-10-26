#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <cstdint>
#include <cstring>

namespace RemoteAccessSystem {
namespace Common {

enum class MessageType : uint32_t {
    // Authentication
    AUTH_REQUEST = 1,
    AUTH_RESPONSE = 2,
    
    // PC Binding
    BIND_REQUEST = 3,
    BIND_RESPONSE = 4,
    UNBIND_REQUEST = 5,
    UNBIND_RESPONSE = 6,
    
    // PC List Management
    PC_LIST_REQUEST = 7,
    PC_LIST_RESPONSE = 8,
    PC_STATUS_UPDATE = 9,
    PC_INFO_REQUEST = 10,
    PC_INFO_RESPONSE = 11,
    
    // File Operations
    FILE_LIST_REQUEST = 20,
    FILE_LIST_RESPONSE = 21,
    FILE_DOWNLOAD_REQUEST = 22,
    FILE_DOWNLOAD_RESPONSE = 23,
    FILE_UPLOAD_REQUEST = 24,
    FILE_UPLOAD_RESPONSE = 25,
    FILE_DELETE_REQUEST = 26,
    FILE_DELETE_RESPONSE = 27,
    
    // File Sharing
    SHARE_LINK_REQUEST = 30,
    SHARE_LINK_RESPONSE = 31,
    
    // Session Management
    SESSION_CREATE = 40,
    SESSION_VALIDATE = 41,
    SESSION_DESTROY = 42,
    HEARTBEAT = 43,
    
    // Error Handling
    ERROR_RESPONSE = 99,
    
    // Keep-Alive
    PING = 100,
    PONG = 101
};

enum class ResponseStatus : uint32_t {
    SUCCESS = 0,
    FAILURE = 1,
    UNAUTHORIZED = 2,
    NOT_FOUND = 3,
    INVALID_REQUEST = 4,
    SERVER_ERROR = 5,
    TIMEOUT = 6
};

struct MessageHeader {
    MessageType type;
    uint64_t message_id;
    uint64_t timestamp;
    ResponseStatus status;  // For response messages
    
    MessageHeader() : type(MessageType::PING), message_id(0), timestamp(0), status(ResponseStatus::SUCCESS) {}
};

struct Message {
    MessageHeader header;
    std::vector<uint8_t> payload;
    
    // Serialization methods
    std::vector<uint8_t> Serialize() const {
        std::vector<uint8_t> result;
        
        // Reserve space for header + payload
        result.reserve(sizeof(uint32_t) * 2 + sizeof(uint64_t) * 2 + sizeof(uint64_t) + payload.size());
        
        // Serialize MessageType
        uint32_t type_value = static_cast<uint32_t>(header.type);
        result.insert(result.end(), 
                     reinterpret_cast<const uint8_t*>(&type_value),
                     reinterpret_cast<const uint8_t*>(&type_value) + sizeof(type_value));
        
        // Serialize message_id
        result.insert(result.end(),
                     reinterpret_cast<const uint8_t*>(&header.message_id),
                     reinterpret_cast<const uint8_t*>(&header.message_id) + sizeof(header.message_id));
        
        // Serialize timestamp
        result.insert(result.end(),
                     reinterpret_cast<const uint8_t*>(&header.timestamp),
                     reinterpret_cast<const uint8_t*>(&header.timestamp) + sizeof(header.timestamp));
        
        // Serialize status
        uint32_t status_value = static_cast<uint32_t>(header.status);
        result.insert(result.end(),
                     reinterpret_cast<const uint8_t*>(&status_value),
                     reinterpret_cast<const uint8_t*>(&status_value) + sizeof(status_value));
        
        // Serialize payload size
        uint64_t payload_size = payload.size();
        result.insert(result.end(),
                     reinterpret_cast<const uint8_t*>(&payload_size),
                     reinterpret_cast<const uint8_t*>(&payload_size) + sizeof(payload_size));
        
        // Serialize payload
        result.insert(result.end(), payload.begin(), payload.end());
        
        return result;
    }
    
    bool Deserialize(const std::vector<uint8_t>& data) {
        size_t min_size = sizeof(uint32_t) * 2 + sizeof(uint64_t) * 3;
        if (data.size() < min_size) {
            return false;
        }
        
        size_t offset = 0;
        
        // Deserialize MessageType
        uint32_t type_value;
        std::memcpy(&type_value, data.data() + offset, sizeof(type_value));
        header.type = static_cast<MessageType>(type_value);
        offset += sizeof(type_value);
        
        // Deserialize message_id
        std::memcpy(&header.message_id, data.data() + offset, sizeof(header.message_id));
        offset += sizeof(header.message_id);
        
        // Deserialize timestamp
        std::memcpy(&header.timestamp, data.data() + offset, sizeof(header.timestamp));
        offset += sizeof(header.timestamp);
        
        // Deserialize status
        uint32_t status_value;
        std::memcpy(&status_value, data.data() + offset, sizeof(status_value));
        header.status = static_cast<ResponseStatus>(status_value);
        offset += sizeof(status_value);
        
        // Deserialize payload size
        uint64_t payload_size;
        std::memcpy(&payload_size, data.data() + offset, sizeof(payload_size));
        offset += sizeof(payload_size);
        
        // Check if we have enough data for payload
        if (data.size() < offset + payload_size) {
            return false;
        }
        
        // Deserialize payload
        payload.clear();
        payload.insert(payload.end(), 
                      data.begin() + offset, 
                      data.begin() + offset + payload_size);
        
        return true;
    }
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // MESSAGE_H
