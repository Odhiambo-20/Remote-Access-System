#include "protocol.h"
#include <sstream>
#include <iomanip>

namespace RemoteAccessSystem {

std::string Message::serialize() const {
    std::ostringstream oss;
    oss << static_cast<uint8_t>(type) << "|" << payload;
    return oss.str();
}

Message Message::deserialize(const std::string& data) {
    Message msg;
    size_t pos = data.find('|');
    if (pos != std::string::npos) {
        msg.type = static_cast<MessageType>(std::stoi(data.substr(0, pos)));
        msg.payload = data.substr(pos + 1);
    }
    return msg;
}

std::string FileInfo::serialize() const {
    std::ostringstream oss;
    oss << name << "|" << path << "|" << size << "|" 
        << (is_directory ? "1" : "0") << "|" << modified_time;
    return oss.str();
}

FileInfo FileInfo::deserialize(const std::string& data) {
    FileInfo info;
    std::istringstream iss(data);
    std::string token;
    
    std::getline(iss, info.name, '|');
    std::getline(iss, info.path, '|');
    std::getline(iss, token, '|');
    info.size = std::stoull(token);
    std::getline(iss, token, '|');
    info.is_directory = (token == "1");
    std::getline(iss, info.modified_time, '|');
    
    return info;
}

std::string ScreenFrame::serialize() const {
    std::ostringstream oss;
    oss << width << "|" << height << "|" << format << "|";
    oss << data.size() << "|";
    oss.write(reinterpret_cast<const char*>(data.data()), data.size());
    return oss.str();
}

ScreenFrame ScreenFrame::deserialize(const std::string& data) {
    ScreenFrame frame;
    std::istringstream iss(data);
    std::string token;
    
    std::getline(iss, token, '|');
    frame.width = std::stoul(token);
    std::getline(iss, token, '|');
    frame.height = std::stoul(token);
    std::getline(iss, token, '|');
    frame.format = std::stoul(token);
    std::getline(iss, token, '|');
    size_t data_size = std::stoull(token);
    
    frame.data.resize(data_size);
    iss.read(reinterpret_cast<char*>(frame.data.data()), data_size);
    
    return frame;
}

std::string MouseEvent::serialize() const {
    std::ostringstream oss;
    oss << static_cast<uint8_t>(type) << "|" << x << "|" << y << "|" << scroll_delta;
    return oss.str();
}

MouseEvent MouseEvent::deserialize(const std::string& data) {
    MouseEvent event;
    std::istringstream iss(data);
    std::string token;
    
    std::getline(iss, token, '|');
    event.type = static_cast<Type>(std::stoi(token));
    std::getline(iss, token, '|');
    event.x = std::stoi(token);
    std::getline(iss, token, '|');
    event.y = std::stoi(token);
    std::getline(iss, token, '|');
    event.scroll_delta = std::stoi(token);
    
    return event;
}

std::string KeyboardEvent::serialize() const {
    std::ostringstream oss;
    oss << static_cast<uint8_t>(type) << "|" << key_code << "|" << modifiers;
    return oss.str();
}

KeyboardEvent KeyboardEvent::deserialize(const std::string& data) {
    KeyboardEvent event;
    std::istringstream iss(data);
    std::string token;
    
    std::getline(iss, token, '|');
    event.type = static_cast<Type>(std::stoi(token));
    std::getline(iss, token, '|');
    event.key_code = std::stoul(token);
    std::getline(iss, token, '|');
    event.modifiers = std::stoul(token);
    
    return event;
}

} // namespace RemoteAccessSystem
