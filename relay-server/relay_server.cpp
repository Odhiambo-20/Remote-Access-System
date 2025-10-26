#include "relay_server.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_socket.h>
#include <sstream>
#include <vector>

RelayServer::RelayServer()
    : running_(false)
    , relay_port_(2810)
    , file_port_(2811)
    , http_port_(8080) {
}

RelayServer::~RelayServer() {
    shutdown();
}

int RelayServer::open(int relay_port, int file_port, int http_port) {
    relay_port_ = relay_port;
    file_port_ = file_port;
    http_port_ = http_port;
    
    // Use IPv4 explicitly with INADDR_ANY
    ACE_INET_Addr relay_addr(relay_port_, (ACE_UINT32)INADDR_ANY);
    ACE_INET_Addr file_addr(file_port_, (ACE_UINT32)INADDR_ANY);
    
    // Open with reuse_addr flag = 1
    if (relay_acceptor_.open(relay_addr, 1) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "Failed to open relay acceptor on port %d\n", relay_port_), -1);
    }
    
    if (file_acceptor_.open(file_addr, 1) == -1) {
        relay_acceptor_.close();
        ACE_ERROR_RETURN((LM_ERROR, "Failed to open file acceptor on port %d\n", file_port_), -1);
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Listening on port %d\n", relay_port_));
    ACE_DEBUG((LM_INFO, "[FileTransfer] Listening on port %d\n", file_port_));
    ACE_DEBUG((LM_INFO, "[HTTP] Server listening on port %d\n", http_port_));
    
    running_ = true;
    return 0;
}

int RelayServer::run() {
    while (running_) {
        ACE_SOCK_Stream* client_stream = new ACE_SOCK_Stream();
        
        if (relay_acceptor_.accept(*client_stream) == -1) {
            delete client_stream;
            if (running_) {
                ACE_ERROR((LM_ERROR, "Failed to accept connection\n"));
            }
            continue;
        }
        
        ACE_DEBUG((LM_INFO, "[RelayServer] New client connected\n"));
        handle_client(client_stream);
    }
    
    return 0;
}

void RelayServer::shutdown() {
    running_ = false;
    relay_acceptor_.close();
    file_acceptor_.close();
    
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    for (auto& pair : registered_pcs_) {
        if (pair.second.connection) {
            pair.second.connection->close();
            delete pair.second.connection;
        }
        if (pair.second.file_connection) {
            pair.second.file_connection->close();
            delete pair.second.file_connection;
        }
    }
    registered_pcs_.clear();
}

int RelayServer::handle_client(ACE_SOCK_Stream* client_stream) {
    char buffer[4096];
    ssize_t bytes = client_stream->recv(buffer, sizeof(buffer) - 1);
    
    if (bytes <= 0) {
        client_stream->close();
        delete client_stream;
        return -1;
    }
    
    buffer[bytes] = '\0';
    std::string request(buffer);
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Received: '%s'\n", request.c_str()));
    
    handle_request(client_stream, request);
    
    return 0;
}

void RelayServer::handle_request(ACE_SOCK_Stream* client_stream, const std::string& request) {
    // Support both old and new command formats
    if (request.find("REGISTER_PC|") == 0) {
        handle_pc_registration(client_stream, request.substr(12));
    }
    else if (request.find("PC_FILE|") == 0) {
        handle_file_handler_registration(client_stream, request.substr(8));
    }
    else if (request == "QUERY_PC_LIST\n" || request == "QUERY_PC_LIST" || 
             request == "GET_PCS\n" || request == "GET_PCS") {
        handle_pc_list_query(client_stream);
    }
    else if (request.find("LIST_DIR|") == 0) {
        handle_list_dir_request(client_stream, request.substr(9));
    }
    else if (request.find("DOWNLOAD_FILE|") == 0) {
        handle_download_request(client_stream, request.substr(14));
    }
    else if (request.find("DIR_LISTING|") == 0 || request.find("FILE_DATA|") == 0) {
        handle_pc_response(client_stream, request);
    }
    else if (request.find("HEARTBEAT") == 0) {
        std::string response = "PONG\n";
        client_stream->send(response.c_str(), response.length());
        ACE_DEBUG((LM_DEBUG, "[RelayServer] Heartbeat acknowledged\n"));
        // Don't close - keep connection alive for heartbeats
    }
    else if (request.find("DIR_LIST|") == 0 || request.find("DOWNLOAD_START|") == 0 || 
             request.find("ERROR|") == 0 || request.find("UPLOAD_COMPLETE") == 0) {
        // Response from file handler - forward to waiting client
        handle_file_handler_response(client_stream, request);
    }
    else {
        ACE_DEBUG((LM_WARNING, "[RelayServer] Unknown command: '%s'\n", request.c_str()));
        std::string error = "ERROR|Unknown command\n";
        client_stream->send(error.c_str(), error.length());
        client_stream->close();
        delete client_stream;
    }
}

void RelayServer::handle_file_handler_registration(ACE_SOCK_Stream* client, const std::string& data) {
    // Format: PC-xxx
    std::string pc_id = data;
    
    // Remove trailing newline if present
    if (!pc_id.empty() && pc_id.back() == '\n') {
        pc_id.pop_back();
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] FileHandler registering for PC: '%s'\n", pc_id.c_str()));
    
    {
        ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
        auto it = registered_pcs_.find(pc_id);
        if (it != registered_pcs_.end()) {
            // PC already registered, add file handler connection
            if (it->second.file_connection) {
                // Close old file connection
                it->second.file_connection->close();
                delete it->second.file_connection;
            }
            it->second.file_connection = client;
            it->second.last_seen = time(nullptr);
            
            std::string response = "OK|FILE_HANDLER_REGISTERED\n";
            client->send(response.c_str(), response.length());
            ACE_DEBUG((LM_INFO, "[RelayServer] FileHandler registered for PC: %s\n", pc_id.c_str()));
            
            // Keep connection open - don't delete or close
            return;
        } else {
            // PC not yet registered, create entry with file connection only
            PCInfo info;
            info.pc_id = pc_id;
            info.connection = nullptr;
            info.file_connection = client;
            info.last_seen = time(nullptr);
            registered_pcs_[pc_id] = info;
            
            std::string response = "OK|FILE_HANDLER_REGISTERED\n";
            client->send(response.c_str(), response.length());
            ACE_DEBUG((LM_INFO, "[RelayServer] FileHandler pre-registered for PC: %s\n", pc_id.c_str()));
            
            // Keep connection open - don't delete or close
            return;
        }
    }
}

void RelayServer::handle_file_handler_response(ACE_SOCK_Stream* file_handler, const std::string& response) {
    ACE_DEBUG((LM_INFO, "[RelayServer] FileHandler response: %s\n", response.c_str()));
    
    // For now, just acknowledge - implement full forwarding later if needed
    // This prevents the "Unknown command" error
}

void RelayServer::handle_pc_registration(ACE_SOCK_Stream* client, const std::string& data) {
    // Support both formats:
    // Old: PC-xxx|user|hostname
    // New: PC-xxx,user,hostname
    
    std::string pc_id, username, hostname;
    
    // Try pipe delimiter first
    size_t pos1 = data.find('|');
    if (pos1 != std::string::npos) {
        pc_id = data.substr(0, pos1);
        size_t pos2 = data.find('|', pos1 + 1);
        if (pos2 != std::string::npos) {
            username = data.substr(pos1 + 1, pos2 - pos1 - 1);
            hostname = data.substr(pos2 + 1);
        }
    } else {
        // Try comma delimiter
        pos1 = data.find(',');
        if (pos1 != std::string::npos) {
            pc_id = data.substr(0, pos1);
            size_t pos2 = data.find(',', pos1 + 1);
            if (pos2 != std::string::npos) {
                username = data.substr(pos1 + 1, pos2 - pos1 - 1);
                hostname = data.substr(pos2 + 1);
            }
        }
    }
    
    if (pc_id.empty() || username.empty() || hostname.empty()) {
        std::string error = "ERROR|Invalid REGISTER format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    // Remove trailing newline if present
    if (!hostname.empty() && hostname.back() == '\n') {
        hostname.pop_back();
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Registering PC: '%s', '%s', '%s'\n", 
              pc_id.c_str(), username.c_str(), hostname.c_str()));
    
    {
        ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
        auto it = registered_pcs_.find(pc_id);
        if (it != registered_pcs_.end()) {
            // Update existing entry
            it->second.username = username;
            it->second.hostname = hostname;
            if (it->second.connection) {
                it->second.connection->close();
                delete it->second.connection;
            }
            it->second.connection = client;
            it->second.last_seen = time(nullptr);
        } else {
            // Create new entry
            PCInfo info;
            info.pc_id = pc_id;
            info.username = username;
            info.hostname = hostname;
            info.connection = client;
            info.file_connection = nullptr;
            info.last_seen = time(nullptr);
            registered_pcs_[pc_id] = info;
        }
    }
    
    std::string response = "OK|Registered\n";
    client->send(response.c_str(), response.length());
    ACE_DEBUG((LM_INFO, "[RelayServer] PC registered: %s\n", hostname.c_str()));
}

void RelayServer::handle_pc_list_query(ACE_SOCK_Stream* client) {
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    
    std::ostringstream oss;
    oss << "PC_LIST|";
    
    for (const auto& pair : registered_pcs_) {
        // Only list PCs that have main connection (fully registered)
        if (pair.second.connection) {
            oss << pair.second.pc_id << "," 
                << pair.second.username << "," 
                << pair.second.hostname << ";";
        }
    }
    oss << "\n";
    
    std::string response = oss.str();
    client->send(response.c_str(), response.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Sent PC list: %d PCs\n", (int)registered_pcs_.size()));
    
    client->close();
    delete client;
}

void RelayServer::handle_list_dir_request(ACE_SOCK_Stream* client, const std::string& data) {
    size_t pos = data.find('|');
    if (pos == std::string::npos) {
        std::string error = "ERROR|Invalid LIST_DIR format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string pc_id = data.substr(0, pos);
    std::string path = data.substr(pos + 1);
    
    if (!path.empty() && path.back() == '\n') {
        path.pop_back();
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] LIST_DIR: PC=%s, path=%s\n", 
              pc_id.c_str(), path.c_str()));
    
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    auto it = registered_pcs_.find(pc_id);
    if (it == registered_pcs_.end() || it->second.file_connection == nullptr) {
        std::string error = "ERROR|PC file handler not found\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string request_id = pc_id + "_" + std::to_string(time(nullptr));
    PendingRequest pending;
    pending.mobile_client = client;
    pending.request_type = "LIST_DIR";
    pending.request_data = path;
    pending.timestamp = time(nullptr);
    pending_requests_[request_id] = pending;
    
    // Send to file handler connection
    std::string request = "LIST_DIR|" + pc_id + "|" + path + "\n";
    it->second.file_connection->send(request.c_str(), request.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Forwarded LIST_DIR to FileHandler\n"));
}

void RelayServer::handle_download_request(ACE_SOCK_Stream* client, const std::string& data) {
    size_t pos = data.find('|');
    if (pos == std::string::npos) {
        std::string error = "ERROR|Invalid DOWNLOAD format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string pc_id = data.substr(0, pos);
    std::string filepath = data.substr(pos + 1);
    
    if (!filepath.empty() && filepath.back() == '\n') {
        filepath.pop_back();
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] DOWNLOAD: PC=%s, file=%s\n", 
              pc_id.c_str(), filepath.c_str()));
    
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    auto it = registered_pcs_.find(pc_id);
    if (it == registered_pcs_.end() || it->second.file_connection == nullptr) {
        std::string error = "ERROR|PC file handler not found\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string request_id = pc_id + "_" + std::to_string(time(nullptr));
    PendingRequest pending;
    pending.mobile_client = client;
    pending.request_type = "DOWNLOAD_FILE";
    pending.request_data = filepath;
    pending.timestamp = time(nullptr);
    pending_requests_[request_id] = pending;
    
    // Send to file handler connection
    std::string request = "DOWNLOAD|" + pc_id + "|" + filepath + "\n";
    it->second.file_connection->send(request.c_str(), request.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Forwarded DOWNLOAD to FileHandler\n"));
}

void RelayServer::handle_pc_response(ACE_SOCK_Stream* pc_client, const std::string& response) {
    ACE_DEBUG((LM_INFO, "[RelayServer] PC response received\n"));
    
    std::string request_id;
    if (response.find("DIR_LISTING|") == 0) {
        size_t pos = response.find('|', 12);
        if (pos != std::string::npos) {
            request_id = response.substr(12, pos - 12);
        }
    } else if (response.find("FILE_DATA|") == 0) {
        size_t pos = response.find('|', 10);
        if (pos != std::string::npos) {
            request_id = response.substr(10, pos - 10);
        }
    }
    
    if (request_id.empty()) {
        ACE_DEBUG((LM_ERROR, "[RelayServer] No request_id in response\n"));
        return;
    }
    
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    auto it = pending_requests_.find(request_id);
    if (it == pending_requests_.end()) {
        ACE_DEBUG((LM_ERROR, "[RelayServer] Unknown request_id: %s\n", request_id.c_str()));
        return;
    }
    
    ACE_SOCK_Stream* mobile_client = it->second.mobile_client;
    mobile_client->send(response.c_str(), response.length());
    mobile_client->close();
    delete mobile_client;
    
    pending_requests_.erase(it);
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Response forwarded to mobile\n"));
}