#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include <cstdint>

namespace RemoteAccessSystem {

// Message types
enum class MessageType : uint8_t {
    // Authentication
    AUTH_REQUEST = 1,
    AUTH_RESPONSE = 2,
    
    // PC Registration
    REGISTER = 10,
    REGISTER_RESPONSE = 11,
    HEARTBEAT = 12,
    HEARTBEAT_RESPONSE = 13,
    DISCONNECT = 14,
    
    // PC List
    GET_PCS = 20,
    PC_LIST = 21,
    
    // Connection Management
    CONNECT_REQUEST = 30,
    CONNECT_RESPONSE = 31,
    DISCONNECT_REQUEST = 32,
    
    // File Operations
    FILE_LIST_REQUEST = 40,
    FILE_LIST_RESPONSE = 41,
    FILE_DOWNLOAD_REQUEST = 42,
    FILE_DOWNLOAD_RESPONSE = 43,
    FILE_UPLOAD_REQUEST = 44,
    FILE_UPLOAD_RESPONSE = 45,
    FILE_DELETE_REQUEST = 46,
    FILE_DELETE_RESPONSE = 47,
    
    // Remote Control
    SCREEN_SHARE_REQUEST = 50,
    SCREEN_SHARE_RESPONSE = 51,
    SCREEN_FRAME = 52,
    MOUSE_EVENT = 53,
    KEYBOARD_EVENT = 54,
    
    // Error
    ERROR = 255
};

// Message structure
struct Message {
    MessageType type;
    std::string payload;
    
    Message() : type(MessageType::ERROR) {}
    Message(MessageType t, const std::string& p) : type(t), payload(p) {}
    
    std::string serialize() const;
    static Message deserialize(const std::string& data);
};

// File info structure
struct FileInfo {
    std::string name;
    std::string path;
    uint64_t size;
    bool is_directory;
    std::string modified_time;
    
    std::string serialize() const;
    static FileInfo deserialize(const std::string& data);
};

// Screen frame structure
struct ScreenFrame {
    uint32_t width;
    uint32_t height;
    uint32_t format; // 0=RGB, 1=JPEG, 2=PNG
    std::vector<uint8_t> data;
    
    std::string serialize() const;
    static ScreenFrame deserialize(const std::string& data);
};

// Mouse event structure
struct MouseEvent {
    enum class Type : uint8_t {
        MOVE = 0,
        LEFT_DOWN = 1,
        LEFT_UP = 2,
        RIGHT_DOWN = 3,
        RIGHT_UP = 4,
        MIDDLE_DOWN = 5,
        MIDDLE_UP = 6,
        SCROLL = 7
    };
    
    Type type;
    int32_t x;
    int32_t y;
    int32_t scroll_delta;
    
    std::string serialize() const;
    static MouseEvent deserialize(const std::string& data);
};

// Keyboard event structure
struct KeyboardEvent {
    enum class Type : uint8_t {
        KEY_DOWN = 0,
        KEY_UP = 1
    };
    
    Type type;
    uint32_t key_code;
    uint32_t modifiers; // Ctrl, Shift, Alt, etc.
    
    std::string serialize() const;
    static KeyboardEvent deserialize(const std::string& data);
};

} // namespace RemoteAccessSystem

#endif // PROTOCOL_H
