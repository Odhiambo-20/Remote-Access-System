#include "relay_client.h"
#include "../../common/include/utils.h"
#include "../../common/include/crypto.h"
#include "file_manager.h"
#include <ace/Log_Msg.h>
#include <numeric>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace RemoteAccessSystem {
namespace Common {

RelayClient::RelayClient()
    : socket_()
    , crypto_() {
    key_ = Crypto::GenerateKey();
    iv_ = Crypto::GenerateIV();
}

RelayClient::~RelayClient() {
    if (socket_.get_handle() != ACE_INVALID_HANDLE) {
        socket_.close();
    }
}

bool RelayClient::Connect(const std::string& relay_address) {
    ACE_INET_Addr addr(relay_address.c_str());
    ACE_SOCK_Connector connector;
    
    if (connector.connect(socket_, addr) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Failed to connect to relay server\n")), false);
    }
    
    ACE_DEBUG((LM_INFO, ACE_TEXT("Connected to relay server: %s\n"), relay_address.c_str()));
    return true;
}

bool RelayClient::Register(const std::string& pc_id) {
    Message msg;
    msg.type = MessageType::AUTH_REQUEST;
    msg.message_id = Utils::GetTimestamp();
    msg.timestamp = Utils::GetTimestamp();
    msg.payload = Utils::StringToBytes(pc_id);

    return SendMessage(msg);
}

bool RelayClient::SendMessage(const Message& msg) {
    // Serialize message
    std::vector<unsigned char> data = SerializeMessage(msg);
    
    // Encrypt data
    std::vector<unsigned char> encrypted = crypto_.Encrypt(data, key_, iv_);
    
    // Send size first
    uint32_t size = encrypted.size();
    if (socket_.send_n(&size, sizeof(size)) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Failed to send message size\n")), false);
    }
    
    // Send encrypted data
    if (socket_.send_n(encrypted.data(), encrypted.size()) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Failed to send message data\n")), false);
    }
    
    return true;
}

bool RelayClient::ReceiveMessage(Message& msg) {
    // Receive size
    uint32_t size;
    if (socket_.recv_n(&size, sizeof(size)) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Failed to receive message size\n")), false);
    }
    
    // Receive encrypted data
    std::vector<unsigned char> encrypted(size);
    if (socket_.recv_n(encrypted.data(), size) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Failed to receive message data\n")), false);
    }
    
    // Decrypt data
    std::vector<unsigned char> data = crypto_.Decrypt(encrypted, key_, iv_);
    
    // Deserialize message
    msg = DeserializeMessage(data);
    return true;
}

bool RelayClient::HandleMessage(const Message& message, Message& response) {
    FileManager fm;
    response.message_id = Utils::GetTimestamp();
    response.timestamp = Utils::GetTimestamp();

    switch (message.type) {
        case MessageType::FILE_LIST_REQUEST: {
            auto path = Utils::BytesToString(message.payload);
            auto files = fm.ListDirectory(path);
            response.type = MessageType::FILE_LIST_RESPONSE;
            
            std::string fileList = std::accumulate(files.begin(), files.end(), std::string(),
                [](const std::string& a, const std::string& b) { 
                    return a.empty() ? b : a + "\n" + b; 
                });
            response.payload = Utils::StringToBytes(fileList);
            break;
        }
        
        case MessageType::FILE_DOWNLOAD: {
            auto path = Utils::BytesToString(message.payload);
            return HandleDownload(path, response);
        }
        
        case MessageType::FILE_UPLOAD: {
            std::string data = Utils::BytesToString(message.payload);
            size_t pos = data.find(':');
            if (pos != std::string::npos) {
                std::string path = data.substr(0, pos);
                std::string content = data.substr(pos + 1);
                fm.WriteFile(path, Utils::StringToBytes(content));
                response.type = MessageType::FILE_UPLOAD;
                response.payload = Utils::StringToBytes("SUCCESS");
            } else {
                response.type = MessageType::FILE_UPLOAD;
                response.payload = Utils::StringToBytes("ERROR|Invalid upload format");
            }
            break;
        }
        
        case MessageType::FILE_COPY: {
            std::string data = Utils::BytesToString(message.payload);
            size_t pos = data.find('|');
            if (pos != std::string::npos) {
                std::string source = data.substr(0, pos);
                std::string dest = data.substr(pos + 1);
                return HandleCopy(source, dest, response);
            } else {
                response.type = MessageType::FILE_COPY;
                response.payload = Utils::StringToBytes("ERROR|Invalid copy parameters");
            }
            break;
        }
        
        case MessageType::FILE_MOVE: {
            std::string data = Utils::BytesToString(message.payload);
            size_t pos = data.find('|');
            if (pos != std::string::npos) {
                std::string source = data.substr(0, pos);
                std::string dest = data.substr(pos + 1);
                return HandleMove(source, dest, response);
            } else {
                response.type = MessageType::FILE_MOVE;
                response.payload = Utils::StringToBytes("ERROR|Invalid move parameters");
            }
            break;
        }
        
        case MessageType::FILE_RENAME: {
            std::string data = Utils::BytesToString(message.payload);
            size_t pos = data.find('|');
            if (pos != std::string::npos) {
                std::string old_path = data.substr(0, pos);
                std::string new_path = data.substr(pos + 1);
                return HandleRename(old_path, new_path, response);
            } else {
                response.type = MessageType::FILE_RENAME;
                response.payload = Utils::StringToBytes("ERROR|Invalid rename parameters");
            }
            break;
        }
        
        case MessageType::FILE_DELETE: {
            auto path = Utils::BytesToString(message.payload);
            return HandleDelete(path, response);
        }
        
        case MessageType::CREATE_FOLDER: {
            auto path = Utils::BytesToString(message.payload);
            return HandleCreateFolder(path, response);
        }
        
        default:
            ACE_ERROR((LM_ERROR, ACE_TEXT("Unknown message type: %d\n"), static_cast<int>(message.type)));
            return false;
    }
    return true;
}

bool RelayClient::HandleDownload(const std::string& file_path, Message& response) {
    try {
        if (!fs::exists(file_path)) {
            response.type = MessageType::FILE_DOWNLOAD;
            response.payload = Utils::StringToBytes("ERROR|File not found");
            return false;
        }
        
        if (!fs::is_regular_file(file_path)) {
            response.type = MessageType::FILE_DOWNLOAD;
            response.payload = Utils::StringToBytes("ERROR|Path is not a file");
            return false;
        }
        
        FileManager fm;
        response.type = MessageType::FILE_DOWNLOAD;
        response.payload = fm.ReadFile(file_path);
        
        ACE_DEBUG((LM_INFO, ACE_TEXT("Download completed: %s\n"), file_path.c_str()));
        return true;
        
    } catch (const std::exception& e) {
        response.type = MessageType::FILE_DOWNLOAD;
        response.payload = Utils::StringToBytes(std::string("ERROR|") + e.what());
        ACE_ERROR((LM_ERROR, ACE_TEXT("Download error: %s\n"), e.what()));
        return false;
    }
}

bool RelayClient::HandleCopy(const std::string& source_path, const std::string& dest_path, Message& response) {
    try {
        if (!fs::exists(source_path)) {
            response.type = MessageType::FILE_COPY;
            response.payload = Utils::StringToBytes("ERROR|Source file not found");
            return false;
        }
        
        fs::path dest(dest_path);
        
        // If destination is a directory, preserve filename
        if (fs::is_directory(dest)) {
            dest = dest / fs::path(source_path).filename();
        }
        
        // Copy file or directory
        if (fs::is_regular_file(source_path)) {
            fs::copy_file(source_path, dest, fs::copy_options::overwrite_existing);
        } else if (fs::is_directory(source_path)) {
            fs::copy(source_path, dest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        }
        
        response.type = MessageType::FILE_COPY;
        response.payload = Utils::StringToBytes("COPY_SUCCESS");
        
        ACE_DEBUG((LM_INFO, ACE_TEXT("Copy completed: %s -> %s\n"), 
                   source_path.c_str(), dest.string().c_str()));
        return true;
        
    } catch (const std::exception& e) {
        response.type = MessageType::FILE_COPY;
        response.payload = Utils::StringToBytes(std::string("ERROR|Copy failed: ") + e.what());
        ACE_ERROR((LM_ERROR, ACE_TEXT("Copy error: %s\n"), e.what()));
        return false;
    }
}

bool RelayClient::HandleMove(const std::string& source_path, const std::string& dest_path, Message& response) {
    try {
        if (!fs::exists(source_path)) {
            response.type = MessageType::FILE_MOVE;
            response.payload = Utils::StringToBytes("ERROR|Source file not found");
            return false;
        }
        
        fs::path dest(dest_path);
        
        // If destination is a directory, preserve filename
        if (fs::is_directory(dest)) {
            dest = dest / fs::path(source_path).filename();
        }
        
        fs::rename(source_path, dest);
        
        response.type = MessageType::FILE_MOVE;
        response.payload = Utils::StringToBytes("MOVE_SUCCESS");
        
        ACE_DEBUG((LM_INFO, ACE_TEXT("Move completed: %s -> %s\n"), 
                   source_path.c_str(), dest.string().c_str()));
        return true;
        
    } catch (const std::exception& e) {
        response.type = MessageType::FILE_MOVE;
        response.payload = Utils::StringToBytes(std::string("ERROR|Move failed: ") + e.what());
        ACE_ERROR((LM_ERROR, ACE_TEXT("Move error: %s\n"), e.what()));
        return false;
    }
}

bool RelayClient::HandleRename(const std::string& old_path, const std::string& new_path, Message& response) {
    try {
        if (!fs::exists(old_path)) {
            response.type = MessageType::FILE_RENAME;
            response.payload = Utils::StringToBytes("ERROR|File not found");
            return false;
        }
        
        if (fs::exists(new_path)) {
            response.type = MessageType::FILE_RENAME;
            response.payload = Utils::StringToBytes("ERROR|Destination already exists");
            return false;
        }
        
        fs::rename(old_path, new_path);
        
        response.type = MessageType::FILE_RENAME;
        response.payload = Utils::StringToBytes("RENAME_SUCCESS");
        
        ACE_DEBUG((LM_INFO, ACE_TEXT("Rename completed: %s -> %s\n"), 
                   old_path.c_str(), new_path.c_str()));
        return true;
        
    } catch (const std::exception& e) {
        response.type = MessageType::FILE_RENAME;
        response.payload = Utils::StringToBytes(std::string("ERROR|Rename failed: ") + e.what());
        ACE_ERROR((LM_ERROR, ACE_TEXT("Rename error: %s\n"), e.what()));
        return false;
    }
}

bool RelayClient::HandleDelete(const std::string& file_path, Message& response) {
    try {
        if (!fs::exists(file_path)) {
            response.type = MessageType::FILE_DELETE;
            response.payload = Utils::StringToBytes("ERROR|File not found");
            return false;
        }
        
        if (fs::is_regular_file(file_path)) {
            fs::remove(file_path);
        } else if (fs::is_directory(file_path)) {
            fs::remove_all(file_path);
        }
        
        response.type = MessageType::FILE_DELETE;
        response.payload = Utils::StringToBytes("DELETE_SUCCESS");
        
        ACE_DEBUG((LM_INFO, ACE_TEXT("Delete completed: %s\n"), file_path.c_str()));
        return true;
        
    } catch (const std::exception& e) {
        response.type = MessageType::FILE_DELETE;
        response.payload = Utils::StringToBytes(std::string("ERROR|Delete failed: ") + e.what());
        ACE_ERROR((LM_ERROR, ACE_TEXT("Delete error: %s\n"), e.what()));
        return false;
    }
}

bool RelayClient::HandleCreateFolder(const std::string& folder_path, Message& response) {
    try {
        if (fs::exists(folder_path)) {
            response.type = MessageType::CREATE_FOLDER;
            response.payload = Utils::StringToBytes("ERROR|Folder already exists");
            return false;
        }
        
        fs::create_directories(folder_path);
        
        response.type = MessageType::CREATE_FOLDER;
        response.payload = Utils::StringToBytes("CREATE_FOLDER_SUCCESS");
        
        ACE_DEBUG((LM_INFO, ACE_TEXT("Folder created: %s\n"), folder_path.c_str()));
        return true;
        
    } catch (const std::exception& e) {
        response.type = MessageType::CREATE_FOLDER;
        response.payload = Utils::StringToBytes(std::string("ERROR|Create folder failed: ") + e.what());
        ACE_ERROR((LM_ERROR, ACE_TEXT("Create folder error: %s\n"), e.what()));
        return false;
    }
}

std::vector<unsigned char> RelayClient::SerializeMessage(const Message& msg) {
    std::vector<unsigned char> data;
    
    // Add type
    data.push_back(static_cast<unsigned char>(msg.type));
    
    // Add message_id (8 bytes)
    for (int i = 0; i < 8; ++i) {
        data.push_back((msg.message_id >> (i * 8)) & 0xFF);
    }
    
    // Add timestamp (8 bytes)
    for (int i = 0; i < 8; ++i) {
        data.push_back((msg.timestamp >> (i * 8)) & 0xFF);
    }
    
    // Add payload size (4 bytes)
    uint32_t payload_size = msg.payload.size();
    for (int i = 0; i < 4; ++i) {
        data.push_back((payload_size >> (i * 8)) & 0xFF);
    }
    
    // Add payload
    data.insert(data.end(), msg.payload.begin(), msg.payload.end());
    
    return data;
}

Message RelayClient::DeserializeMessage(const std::vector<unsigned char>& data) {
    Message msg;
    size_t offset = 0;
    
    // Read type
    msg.type = static_cast<MessageType>(data[offset++]);
    
    // Read message_id
    msg.message_id = 0;
    for (int i = 0; i < 8; ++i) {
        msg.message_id |= (static_cast<uint64_t>(data[offset++]) << (i * 8));
    }
    
    // Read timestamp
    msg.timestamp = 0;
    for (int i = 0; i < 8; ++i) {
        msg.timestamp |= (static_cast<uint64_t>(data[offset++]) << (i * 8));
    }
    
    // Read payload size
    uint32_t payload_size = 0;
    for (int i = 0; i < 4; ++i) {
        payload_size |= (static_cast<uint32_t>(data[offset++]) << (i * 8));
    }
    
    // Read payload
    msg.payload.assign(data.begin() + offset, data.begin() + offset + payload_size);
    
    return msg;
}

} // namespace Common
} // namespace RemoteAccessSystem