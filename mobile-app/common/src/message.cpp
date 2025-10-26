#include "../include/message.h"
#include <cstring>
#include <algorithm>

using namespace RemoteAccessSystem::Common;

Message::Message()
    : type_(MessageType::UNKNOWN), payload_("") {
    std::memset(&header_, 0, sizeof(ProtocolHeader));
    header_.magic = 0xDEADBEEF;
    header_.version = 1;
    header_.type = MessageType::UNKNOWN;
}

Message::Message(MessageType type)
    : type_(type), payload_("") {
    std::memset(&header_, 0, sizeof(ProtocolHeader));
    header_.magic = 0xDEADBEEF;
    header_.version = 1;
    header_.type = type;
}

Message::Message(MessageType type, const std::string& payload)
    : type_(type), payload_(payload) {
    std::memset(&header_, 0, sizeof(ProtocolHeader));
    header_.magic = 0xDEADBEEF;
    header_.version = 1;
    header_.type = type;
    header_.payload_size = payload.size();
}

Message::~Message() {
}

void Message::setType(MessageType type) {
    type_ = type;
    header_.type = type;
}

MessageType Message::getType() const {
    return type_;
}

void Message::setPayload(const std::string& payload) {
    payload_ = payload;
    header_.payload_size = payload.size();
}

std::string Message::getPayload() const {
    return payload_;
}

ProtocolHeader& Message::getHeader() {
    return header_;
}

const ProtocolHeader& Message::getHeader() const {
    return header_;
}

std::vector<unsigned char> Message::serialize() const {
    std::vector<unsigned char> data;
    
    const unsigned char* hdr = reinterpret_cast<const unsigned char*>(&header_);
    for (size_t i = 0; i < sizeof(ProtocolHeader); ++i) {
        data.push_back(hdr[i]);
    }
    
    for (char c : payload_) {
        data.push_back(static_cast<unsigned char>(c));
    }
    
    return data;
}

bool Message::deserialize(const std::vector<unsigned char>& data) {
    if (data.size() < sizeof(ProtocolHeader)) {
        return false;
    }
    
    std::memcpy(&header_, data.data(), sizeof(ProtocolHeader));
    
    if (header_.magic != 0xDEADBEEF) {
        return false;
    }
    
    type_ = header_.type;
    
    if (data.size() > sizeof(ProtocolHeader)) {
        payload_.assign(
            reinterpret_cast<const char*>(data.data() + sizeof(ProtocolHeader)),
            data.size() - sizeof(ProtocolHeader)
        );
    } else {
        payload_.clear();
    }
    
    return true;
}
