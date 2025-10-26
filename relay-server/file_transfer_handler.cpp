#include "file_transfer_handler.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_string.h>
#include <sstream>
#include <random>

FileTransferHandler::FileTransferHandler()
    : running_(false) {
}

FileTransferHandler::~FileTransferHandler() {
    shutdown();
}

int FileTransferHandler::init(int port) {
    ACE_INET_Addr addr(port);
    
    if (acceptor_.open(addr, 1) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "Failed to open file transfer acceptor on port %d\n", port), -1);
    }
    
    ACE_DEBUG((LM_INFO, "[FileTransfer] Listening on port %d\n", port));
    running_ = true;
    
    return this->activate(THR_NEW_LWP | THR_JOINABLE);
}

int FileTransferHandler::svc() {
    while (running_) {
        ACE_SOCK_Stream* client_stream = new ACE_SOCK_Stream();
        
        if (acceptor_.accept(*client_stream) == -1) {
            if (running_) {
                ACE_ERROR((LM_ERROR, "[FileTransfer] Accept failed\n"));
            }
            delete client_stream;
            continue;
        }
        
        ACE_DEBUG((LM_INFO, "[FileTransfer] New client connected\n"));
        
        // Handle client in a separate thread
        ACE_Thread_Manager::instance()->spawn(
            (ACE_THR_FUNC)([](void* arg) -> void* {
                FileTransferHandler* handler = static_cast<FileTransferHandler*>(arg);
                ACE_SOCK_Stream* stream = static_cast<ACE_SOCK_Stream*>(arg);
                handler->handle_client(stream);
                return nullptr;
            }),
            this
        );
    }
    
    return 0;
}

void FileTransferHandler::handle_client(ACE_SOCK_Stream* client_stream) {
    char buffer[4096];
    ssize_t bytes_received = client_stream->recv(buffer, sizeof(buffer) - 1);
    
    if (bytes_received <= 0) {
        client_stream->close();
        delete client_stream;
        return;
    }
    
    buffer[bytes_received] = '\0';
    std::string request(buffer);
    
    ACE_DEBUG((LM_INFO, "[FileTransfer] Received: %s\n", request.c_str()));
    
    // Parse request
    if (request.find("MOBILE_FILE|") == 0) {
        // Mobile client connecting for file operations
        size_t pos = request.find('|');
        std::string pc_id = request.substr(pos + 1);
        pc_id = pc_id.substr(0, pc_id.find('\n'));
        
        ACE_DEBUG((LM_INFO, "[FileTransfer] Mobile client for PC: %s\n", pc_id.c_str()));
        
        // Wait for commands
        while (true) {
            bytes_received = client_stream->recv(buffer, sizeof(buffer) - 1);
            if (bytes_received <= 0) break;
            
            buffer[bytes_received] = '\0';
            std::string command(buffer);
            handle_mobile_request(client_stream, pc_id, command);
        }
    }
    else if (request.find("PC_FILE|") == 0) {
        // PC connecting to handle file requests
        size_t pos = request.find('|');
        std::string pc_id = request.substr(pos + 1);
        pc_id = pc_id.substr(0, pc_id.find('\n'));
        
        ACE_DEBUG((LM_INFO, "[FileTransfer] PC registered: %s\n", pc_id.c_str()));
        
        {
            ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
            pc_connections_[pc_id] = client_stream;
        }
        
        // Keep connection alive
        handle_pc_response(client_stream, pc_id);
        
        {
            ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
            pc_connections_.erase(pc_id);
        }
    }
    
    client_stream->close();
    delete client_stream;
}

void FileTransferHandler::handle_mobile_request(ACE_SOCK_Stream* client, 
                                                const std::string& pc_id, 
                                                const std::string& request) {
    ACE_DEBUG((LM_INFO, "[FileTransfer] Mobile request: %s\n", request.c_str()));
    
    ACE_SOCK_Stream* pc_stream = nullptr;
    {
        ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
        auto it = pc_connections_.find(pc_id);
        if (it != pc_connections_.end()) {
            pc_stream = it->second;
        }
    }
    
    if (!pc_stream) {
        std::string error = "ERROR|PC not connected\n";
        client->send(error.c_str(), error.length());
        return;
    }
    
    // Parse command
    if (request.find("LIST_DIR|") == 0) {
        // Forward to PC
        pc_stream->send(request.c_str(), request.length());
        
        // Wait for response from PC
        char buffer[8192];
        ssize_t bytes = pc_stream->recv(buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            client->send(buffer, bytes);
        }
    }
    else if (request.find("DOWNLOAD|") == 0) {
        // Forward to PC
        pc_stream->send(request.c_str(), request.length());
        
        // Relay file data from PC to mobile
        char buffer[8192];
        while (true) {
            ssize_t bytes = pc_stream->recv(buffer, sizeof(buffer));
            if (bytes <= 0) break;
            client->send(buffer, bytes);
        }
    }
    else if (request.find("UPLOAD|") == 0) {
        // Forward to PC
        pc_stream->send(request.c_str(), request.length());
        
        // Relay file data from mobile to PC
        char buffer[8192];
        while (true) {
            ssize_t bytes = client->recv(buffer, sizeof(buffer));
            if (bytes <= 0) break;
            pc_stream->send(buffer, bytes);
        }
        
        std::string ok = "UPLOAD_OK\n";
        client->send(ok.c_str(), ok.length());
    }
    else if (request.find("GENERATE_URL|") == 0) {
        // Parse: GENERATE_URL|pc_id|file_path
        std::vector<std::string> parts;
        std::stringstream ss(request);
        std::string part;
        while (std::getline(ss, part, '|')) {
            parts.push_back(part);
        }
        
        if (parts.size() >= 3) {
            std::string file_path = parts[2];
            file_path = file_path.substr(0, file_path.find('\n'));
            
            std::string url = generate_share_url(pc_id, file_path);
            std::string response = "SHARE_URL|" + url + "\n";
            client->send(response.c_str(), response.length());
            
            ACE_DEBUG((LM_INFO, "[FileTransfer] Generated share URL: %s\n", url.c_str()));
        }
    }
}

void FileTransferHandler::handle_pc_response(ACE_SOCK_Stream* client, const std::string& pc_id) {
    // Keep connection alive and handle responses
    char buffer[8192];
    while (true) {
        ssize_t bytes = client->recv(buffer, sizeof(buffer));
        if (bytes <= 0) break;
        
        // PC responses are handled in handle_mobile_request
        ACE_OS::sleep(1);
    }
}

std::string FileTransferHandler::generate_token() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    
    std::string token;
    for (int i = 0; i < 32; ++i) {
        token += alphanum[dis(gen)];
    }
    
    return token;
}

std::string FileTransferHandler::generate_share_url(const std::string& pc_id, 
                                                    const std::string& file_path) {
    std::string token = generate_token();
    
    FileShareInfo info;
    info.pc_id = pc_id;
    info.file_path = file_path;
    info.share_token = token;
    info.created_at = time(nullptr);
    
    {
        ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
        share_tokens_[token] = info;
    }
    
    // Generate URL (assuming relay server is accessible)
    std::string url = "http://192.168.100.89:8080/share/" + token;
    return url;
}

FileShareInfo* FileTransferHandler::get_share_info(const std::string& token) {
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    auto it = share_tokens_.find(token);
    if (it != share_tokens_.end()) {
        return &(it->second);
    }
    return nullptr;
}

void FileTransferHandler::shutdown() {
    running_ = false;
    acceptor_.close();
    
    {
        ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
        for (auto& pair : pc_connections_) {
            pair.second->close();
        }
        pc_connections_.clear();
    }
    
    this->wait();
}
