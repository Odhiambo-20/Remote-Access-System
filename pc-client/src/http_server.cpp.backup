#include "../include/http_server.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_sys_stat.h>
#include <fstream>
#include <sstream>
#include <map>
#include <thread>
#include <mutex>

namespace RemoteAccessSystem {
namespace Common {

// Global map to store token -> file_path mappings
static std::map<std::string, std::string> share_tokens;
static std::mutex token_mutex;

HTTPServer::HTTPServer() : running_(false) {}

HTTPServer::~HTTPServer() {
    Stop();
}

void AddShareToken(const std::string& token, const std::string& file_path) {
    std::lock_guard<std::mutex> lock(token_mutex);
    share_tokens[token] = file_path;
    ACE_DEBUG((LM_INFO, "[HTTPServer] Added share token: %s -> %s\n", 
              token.c_str(), file_path.c_str()));
}

bool HTTPServer::Start(const std::string& address, uint16_t port) {
    ACE_INET_Addr server_addr(port, address.c_str());
    
    if (acceptor_.open(server_addr, 1) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "[HTTPServer] Failed to open on %s:%d\n", 
                         address.c_str(), port), false);
    }
    
    running_ = true;
    ACE_DEBUG((LM_INFO, "[HTTPServer] Started on %s:%d\n", address.c_str(), port));
    
    // Start server thread
    std::thread([this]() {
        while (running_) {
            ACE_SOCK_Stream client;
            if (acceptor_.accept(client) == 0) {
                // Handle in new thread
                std::thread([this, client]() mutable {
                    HandleRequest(client);
                    client.close();
                }).detach();
            }
        }
    }).detach();
    
    return true;
}

void HTTPServer::Stop() {
    running_ = false;
    acceptor_.close();
    ACE_DEBUG((LM_INFO, "[HTTPServer] Stopped\n"));
}

void HTTPServer::HandleRequest(ACE_SOCK_Stream& client) {
    char buffer[4096];
    ssize_t bytes = client.recv(buffer, sizeof(buffer) - 1);
    
    if (bytes <= 0) {
        return;
    }
    
    buffer[bytes] = '\0';
    std::string request(buffer);
    
    ACE_DEBUG((LM_DEBUG, "[HTTPServer] Request:\n%s\n", request.c_str()));
    
    // Parse HTTP request line
    size_t first_line_end = request.find("\r\n");
    if (first_line_end == std::string::npos) {
        first_line_end = request.find("\n");
    }
    
    std::string request_line = request.substr(0, first_line_end);
    std::istringstream iss(request_line);
    std::string method, path, http_version;
    iss >> method >> path >> http_version;
    
    ACE_DEBUG((LM_INFO, "[HTTPServer] %s %s\n", method.c_str(), path.c_str()));
    
    // Handle /share/TOKEN requests
    if (path.find("/share/") == 0 && path.length() > 7) {
        std::string token = path.substr(7); // Extract token after "/share/"
        
        // Remove query parameters if any
        size_t query_pos = token.find('?');
        if (query_pos != std::string::npos) {
            token = token.substr(0, query_pos);
        }
        
        std::string file_path;
        {
            std::lock_guard<std::mutex> lock(token_mutex);
            auto it = share_tokens.find(token);
            if (it != share_tokens.end()) {
                file_path = it->second;
            }
        }
        
        if (file_path.empty()) {
            // Token not found
            std::string response = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html><body><h1>404 - Share link not found or expired</h1></body></html>";
            client.send(response.c_str(), response.length());
            ACE_ERROR((LM_WARNING, "[HTTPServer] Token not found: %s\n", token.c_str()));
            return;
        }
        
        // Check if file exists
        ACE_stat stat_buf;
        if (ACE_OS::stat(file_path.c_str(), &stat_buf) != 0) {
            std::string response = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html><body><h1>404 - File not found</h1></body></html>";
            client.send(response.c_str(), response.length());
            ACE_ERROR((LM_ERROR, "[HTTPServer] File not found: %s\n", file_path.c_str()));
            return;
        }
        
        // Open and send file
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            std::string response = 
                "HTTP/1.1 500 Internal Server Error\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html><body><h1>500 - Cannot open file</h1></body></html>";
            client.send(response.c_str(), response.length());
            ACE_ERROR((LM_ERROR, "[HTTPServer] Cannot open file: %s\n", file_path.c_str()));
            return;
        }
        
        // Get file size
        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Determine content type based on file extension
        std::string content_type = "application/octet-stream";
        size_t dot_pos = file_path.find_last_of('.');
        if (dot_pos != std::string::npos) {
            std::string ext = file_path.substr(dot_pos + 1);
            if (ext == "pdf") content_type = "application/pdf";
            else if (ext == "jpg" || ext == "jpeg") content_type = "image/jpeg";
            else if (ext == "png") content_type = "image/png";
            else if (ext == "txt") content_type = "text/plain";
            else if (ext == "html" || ext == "htm") content_type = "text/html";
            else if (ext == "doc" || ext == "docx") content_type = "application/msword";
            else if (ext == "xls" || ext == "xlsx") content_type = "application/vnd.ms-excel";
            else if (ext == "ppt" || ext == "pptx") content_type = "application/vnd.ms-powerpoint";
        }
        
        // Extract filename
        std::string filename = file_path.substr(file_path.find_last_of('/') + 1);
        
        // Send HTTP headers
        std::ostringstream header;
        header << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: " << content_type << "\r\n"
               << "Content-Length: " << file_size << "\r\n"
               << "Content-Disposition: inline; filename=\"" << filename << "\"\r\n"
               << "Connection: close\r\n"
               << "\r\n";
        
        std::string header_str = header.str();
        client.send(header_str.c_str(), header_str.length());
        
        ACE_DEBUG((LM_INFO, "[HTTPServer] Serving file: %s (%zu bytes)\n", 
                  file_path.c_str(), file_size));
        
        // Send file data
        char file_buffer[8192];
        size_t total_sent = 0;
        while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
            ssize_t sent = client.send(file_buffer, file.gcount());
            if (sent <= 0) {
                ACE_ERROR((LM_ERROR, "[HTTPServer] Error sending file data\n"));
                break;
            }
            total_sent += sent;
        }
        
        file.close();
        ACE_DEBUG((LM_INFO, "[HTTPServer] File sent successfully: %zu bytes\n", total_sent));
    }
    else {
        // Unknown path
        std::string response = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><h1>404 - Not Found</h1><p>Path: " + path + "</p></body></html>";
        client.send(response.c_str(), response.length());
    }
}

} // namespace Common
} // namespace RemoteAccessSystem

// Export function for file_handler to use
extern "C" {
    void http_server_add_share_token(const char* token, const char* file_path) {
        RemoteAccessSystem::Common::AddShareToken(token, file_path);
    }
}