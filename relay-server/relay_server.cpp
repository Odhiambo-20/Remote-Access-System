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
    else if (request.find("FILE_HANDLER_REGISTER|") == 0) {
        handle_file_handler_registration(client_stream, request.substr(22));
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
    else if (request.find("DOWNLOAD|") == 0) {
        handle_download_request(client_stream, request.substr(9));
    }
    else if (request.find("GENERATE_URL|") == 0) {
        handle_generate_url_request(client_stream, request.substr(13));
    }
    else if (request.find("DELETE|") == 0) {
        handle_delete_request(client_stream, request.substr(7));
    }
    else if (request.find("RENAME|") == 0) {
        handle_rename_request(client_stream, request.substr(7));
    }
    else if (request.find("COPY|") == 0) {
        handle_copy_request(client_stream, request.substr(5));
    }
    else if (request.find("UPLOAD|") == 0) {
        handle_upload_request(client_stream, request.substr(7));
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
    else if (request.find("DIR_LIST|") == 0 || 
             request.find("DOWNLOAD_START|") == 0 || 
             request.find("ERROR|") == 0 || 
             request.find("UPLOAD_COMPLETE") == 0 ||
             request.find("UPLOAD_READY") == 0 ||
             request.find("DELETE_OK") == 0 ||
             request.find("RENAME_OK") == 0 ||
             request.find("COPY_OK") == 0 ||
             request.find("SHARE_URL|") == 0) {
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
    
    // Extract response type
    std::string response_type;
    size_t pipe_pos = response.find('|');
    if (pipe_pos != std::string::npos) {
        response_type = response.substr(0, pipe_pos);
    } else {
        response_type = response;
        if (!response_type.empty() && response_type.back() == '\n') {
            response_type.pop_back();
        }
    }
    
    // Find the most recent pending request that matches this response type
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    
    PendingRequest* matching_request = nullptr;
    std::string matching_id;
    time_t newest_timestamp = 0;
    
    for (auto& pair : pending_requests_) {
        // Match response to request type
        bool matches = false;
        if (response_type == "DIR_LIST" && pair.second.request_type == "LIST_DIR") {
            matches = true;
        } else if (response_type == "DOWNLOAD_START" && pair.second.request_type == "DOWNLOAD_FILE") {
            matches = true;
        } else if (response_type == "SHARE_URL" && pair.second.request_type == "GENERATE_URL") {
            matches = true;
        } else if (response_type == "DELETE_OK" && pair.second.request_type == "DELETE") {
            matches = true;
        } else if (response_type == "RENAME_OK" && pair.second.request_type == "RENAME") {
            matches = true;
        } else if (response_type == "COPY_OK" && pair.second.request_type == "COPY") {
            matches = true;
        } else if (response_type == "UPLOAD_READY" && pair.second.request_type == "UPLOAD") {
            matches = true;
        } else if (response_type == "UPLOAD_COMPLETE" && pair.second.request_type == "UPLOAD") {
            matches = true;
        } else if (response_type == "ERROR") {
            matches = true; // Error responses match any request
        }
        
        if (matches && pair.second.timestamp > newest_timestamp) {
            matching_request = &pair.second;
            matching_id = pair.first;
            newest_timestamp = pair.second.timestamp;
        }
    }
    
    if (matching_request) {
        ACE_SOCK_Stream* mobile_client = matching_request->mobile_client;
        mobile_client->send(response.c_str(), response.length());
        
        // Only close and delete for final responses
        if (response_type == "DIR_LIST" || 
            response_type == "DELETE_OK" || 
            response_type == "RENAME_OK" || 
            response_type == "COPY_OK" ||
            response_type == "SHARE_URL" ||
            response_type == "UPLOAD_COMPLETE" ||
            response_type == "ERROR") {
            mobile_client->close();
            delete mobile_client;
            pending_requests_.erase(matching_id);
            ACE_DEBUG((LM_INFO, "[RelayServer] Response forwarded and request completed\n"));
        } else {
            ACE_DEBUG((LM_INFO, "[RelayServer] Intermediate response forwarded\n"));
        }
    } else {
        ACE_DEBUG((LM_WARNING, "[RelayServer] No matching pending request for response: %s\n", 
                  response_type.c_str()));
    }
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

void RelayServer::handle_generate_url_request(ACE_SOCK_Stream* client, const std::string& data) {
    size_t pos = data.find('|');
    if (pos == std::string::npos) {
        std::string error = "ERROR|Invalid GENERATE_URL format\n";
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
    
    ACE_DEBUG((LM_INFO, "[RelayServer] GENERATE_URL: PC=%s, file=%s\n", 
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
    pending.request_type = "GENERATE_URL";
    pending.request_data = filepath;
    pending.timestamp = time(nullptr);
    pending_requests_[request_id] = pending;
    
    // Send to file handler connection
    std::string request = "GENERATE_URL|" + pc_id + "|" + filepath + "\n";
    it->second.file_connection->send(request.c_str(), request.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Forwarded GENERATE_URL to FileHandler\n"));
}

void RelayServer::handle_delete_request(ACE_SOCK_Stream* client, const std::string& data) {
    size_t pos = data.find('|');
    if (pos == std::string::npos) {
        std::string error = "ERROR|Invalid DELETE format\n";
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
    
    ACE_DEBUG((LM_INFO, "[RelayServer] DELETE: PC=%s, file=%s\n", 
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
    pending.request_type = "DELETE";
    pending.request_data = filepath;
    pending.timestamp = time(nullptr);
    pending_requests_[request_id] = pending;
    
    // Send to file handler connection
    std::string request = "DELETE|" + pc_id + "|" + filepath + "\n";
    it->second.file_connection->send(request.c_str(), request.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Forwarded DELETE to FileHandler\n"));
}

void RelayServer::handle_rename_request(ACE_SOCK_Stream* client, const std::string& data) {
    // Format: PC-xxx|oldpath|newpath
    size_t pos1 = data.find('|');
    if (pos1 == std::string::npos) {
        std::string error = "ERROR|Invalid RENAME format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string pc_id = data.substr(0, pos1);
    size_t pos2 = data.find('|', pos1 + 1);
    if (pos2 == std::string::npos) {
        std::string error = "ERROR|Invalid RENAME format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string old_path = data.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string new_path = data.substr(pos2 + 1);
    
    if (!new_path.empty() && new_path.back() == '\n') {
        new_path.pop_back();
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] RENAME: PC=%s, old=%s, new=%s\n", 
              pc_id.c_str(), old_path.c_str(), new_path.c_str()));
    
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
    pending.request_type = "RENAME";
    pending.request_data = old_path + "|" + new_path;
    pending.timestamp = time(nullptr);
    pending_requests_[request_id] = pending;
    
    // Send to file handler connection
    std::string request = "RENAME|" + pc_id + "|" + old_path + "|" + new_path + "\n";
    it->second.file_connection->send(request.c_str(), request.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Forwarded RENAME to FileHandler\n"));
}

void RelayServer::handle_copy_request(ACE_SOCK_Stream* client, const std::string& data) {
    // Format: PC-xxx|srcpath|destpath
    size_t pos1 = data.find('|');
    if (pos1 == std::string::npos) {
        std::string error = "ERROR|Invalid COPY format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string pc_id = data.substr(0, pos1);
    size_t pos2 = data.find('|', pos1 + 1);
    if (pos2 == std::string::npos) {
        std::string error = "ERROR|Invalid COPY format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string src_path = data.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string dest_path = data.substr(pos2 + 1);
    
    if (!dest_path.empty() && dest_path.back() == '\n') {
        dest_path.pop_back();
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] COPY: PC=%s, src=%s, dest=%s\n", 
              pc_id.c_str(), src_path.c_str(), dest_path.c_str()));
    
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
    pending.request_type = "COPY";
    pending.request_data = src_path + "|" + dest_path;
    pending.timestamp = time(nullptr);
    pending_requests_[request_id] = pending;
    
    // Send to file handler connection
    std::string request = "COPY|" + pc_id + "|" + src_path + "|" + dest_path + "\n";
    it->second.file_connection->send(request.c_str(), request.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Forwarded COPY to FileHandler\n"));
}

void RelayServer::handle_upload_request(ACE_SOCK_Stream* client, const std::string& data) {
    // Format: PC-xxx|remotepath|filesize
    size_t pos1 = data.find('|');
    if (pos1 == std::string::npos) {
        std::string error = "ERROR|Invalid UPLOAD format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string pc_id = data.substr(0, pos1);
    size_t pos2 = data.find('|', pos1 + 1);
    if (pos2 == std::string::npos) {
        std::string error = "ERROR|Invalid UPLOAD format\n";
        client->send(error.c_str(), error.length());
        client->close();
        delete client;
        return;
    }
    
    std::string remote_path = data.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string size_str = data.substr(pos2 + 1);
    
    if (!size_str.empty() && size_str.back() == '\n') {
        size_str.pop_back();
    }
    
    ACE_DEBUG((LM_INFO, "[RelayServer] UPLOAD: PC=%s, path=%s, size=%s\n", 
              pc_id.c_str(), remote_path.c_str(), size_str.c_str()));
    
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
    pending.request_type = "UPLOAD";
    pending.request_data = remote_path + "|" + size_str;
    pending.timestamp = time(nullptr);
    pending_requests_[request_id] = pending;
    
    // Send to file handler connection
    std::string request = "UPLOAD|" + pc_id + "|" + remote_path + "|" + size_str + "\n";
    it->second.file_connection->send(request.c_str(), request.length());
    
    ACE_DEBUG((LM_INFO, "[RelayServer] Forwarded UPLOAD to FileHandler\n"));
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