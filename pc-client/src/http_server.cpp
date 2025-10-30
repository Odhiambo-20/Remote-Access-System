#include "../include/http_server.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_sys_stat.h>
#include <qrencode.h>
#include <png.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <thread>
#include <mutex>
#include <random>
#include <iomanip>
#include <cstring>

namespace RemoteAccessSystem {
namespace Common {

// Global state for PC connection info and shared files
static std::map<std::string, std::string> share_tokens;
static std::mutex token_mutex;
static std::string g_pc_id;
static std::string g_username;
static std::string g_relay_server = "127.0.0.1";
static uint16_t g_relay_port = 2810;
static std::string g_auth_token;

// Constructor & Destructor
HTTPServer::HTTPServer() : running_(false) {}
HTTPServer::~HTTPServer() { Stop(); }

// Set PC information for QR code
void SetPCInfo(const std::string& pc_id, const std::string& username,
               const std::string& relay_server, uint16_t relay_port) {
    g_pc_id = pc_id;
    g_username = username;
    g_relay_server = relay_server;
    g_relay_port = relay_port;
    
    // Generate authentication token if not already set
    if (g_auth_token.empty()) {
        g_auth_token = GenerateAuthToken();
    }
    
    ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] PC info set: %s@%s:%d (token: %s)\n"),
              username.c_str(), relay_server.c_str(), relay_port, g_auth_token.c_str()));
}

// Generate secure authentication token
std::string GenerateAuthToken() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::ostringstream oss;
    for (int i = 0; i < 32; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }
    return oss.str();
}

// Add file sharing token
void AddShareToken(const std::string& token, const std::string& file_path) {
    std::lock_guard<std::mutex> lock(token_mutex);
    share_tokens[token] = file_path;
    ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] Token added: %s -> %s\n"), 
              token.c_str(), file_path.c_str()));
}

// Get connection info for QR code
std::string HTTPServer::GetConnectionInfo() const {
    return GenerateQRData();
}

std::string HTTPServer::GenerateQRData() const {
    std::ostringstream data;
    data << g_pc_id << "|" 
         << g_username << "|" 
         << g_relay_server << "|" 
         << g_relay_port << "|"
         << g_auth_token;
    return data.str();
}

// Start HTTP server
bool HTTPServer::Start(const std::string& address, uint16_t port) {
    ACE_INET_Addr server_addr(port, address.c_str());
    if (acceptor_.open(server_addr, 1) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("[HTTPServer] Failed to open %s:%d\n"),
                         address.c_str(), port), false);
    }
    
    running_ = true;
    ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] Started on %s:%d\n"), address.c_str(), port));
    
    // Start accepting connections in a separate thread
    std::thread([this]() {
        while (running_) {
            ACE_SOCK_Stream client;
            if (acceptor_.accept(client) == 0) {
                std::thread([this, client]() mutable {
                    HandleRequest(client);
                    client.close();
                }).detach();
            }
        }
    }).detach();
    
    return true;
}

// Stop HTTP server
void HTTPServer::Stop() {
    running_ = false;
    acceptor_.close();
    ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] Stopped\n")));
}

// Generate QR code as PNG image
std::vector<uint8_t> HTTPServer::GenerateQRCodePNG(const std::string& data, int size) {
    // Generate QR code using libqrencode
    QRcode* qr = QRcode_encodeString(data.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    if (!qr) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("[HTTPServer] QR code generation failed\n")));
        return {};
    }
    
    int qr_width = qr->width;
    int scale = size / qr_width;
    if (scale < 1) scale = 1;
    int img_size = qr_width * scale;
    
    // Create PNG in memory
    std::vector<uint8_t> png_data;
    
    // Use memory buffer instead of file
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        QRcode_free(qr);
        return {};
    }
    
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        QRcode_free(qr);
        return {};
    }
    
    // Set up custom write function to write to vector
    png_set_write_fn(png, &png_data, 
        [](png_structp png_ptr, png_bytep data, png_size_t length) {
            std::vector<uint8_t>* vec = static_cast<std::vector<uint8_t>*>(png_get_io_ptr(png_ptr));
            vec->insert(vec->end(), data, data + length);
        }, 
        nullptr);
    
    // Set PNG parameters
    png_set_IHDR(png, info, img_size, img_size, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);
    
    // Create image data (white background, black QR modules)
    std::vector<uint8_t> row_data(img_size * 3, 255); // RGB white
    
    for (int y = 0; y < img_size; ++y) {
        int qr_y = y / scale;
        for (int x = 0; x < img_size; ++x) {
            int qr_x = x / scale;
            int idx = qr_y * qr_width + qr_x;
            
            if (qr->data[idx] & 1) {
                // Black module
                row_data[x * 3 + 0] = 0;
                row_data[x * 3 + 1] = 0;
                row_data[x * 3 + 2] = 0;
            } else {
                // White module
                row_data[x * 3 + 0] = 255;
                row_data[x * 3 + 1] = 255;
                row_data[x * 3 + 2] = 255;
            }
        }
        png_write_row(png, row_data.data());
        
        // Reset to white for next row
        std::fill(row_data.begin(), row_data.end(), 255);
    }
    
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    QRcode_free(qr);
    
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("[HTTPServer] Generated QR PNG: %zu bytes\n"), png_data.size()));
    return png_data;
}

// Send error response
void HTTPServer::SendErrorResponse(ACE_SOCK_Stream& client, int code, const std::string& message) {
    std::ostringstream response;
    response << "HTTP/1.1 " << code << " ";
    
    switch (code) {
        case 404: response << "Not Found"; break;
        case 500: response << "Internal Server Error"; break;
        default: response << "Error"; break;
    }
    
    response << "\r\n"
             << "Content-Type: text/plain\r\n"
             << "Content-Length: " << message.length() << "\r\n"
             << "\r\n"
             << message;
    
    std::string resp = response.str();
    client.send(resp.c_str(), resp.length());
}

// Send file response
void HTTPServer::SendFileResponse(ACE_SOCK_Stream& client, const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        SendErrorResponse(client, 404, "File not found");
        return;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);
    
    // Extract filename from path
    size_t pos = file_path.find_last_of("/\\");
    std::string filename = (pos != std::string::npos) ? file_path.substr(pos + 1) : file_path;
    
    // Send headers
    std::ostringstream header;
    header << "HTTP/1.1 200 OK\r\n"
           << "Content-Type: application/octet-stream\r\n"
           << "Content-Disposition: attachment; filename=\"" << filename << "\"\r\n"
           << "Content-Length: " << size << "\r\n"
           << "\r\n";
    
    std::string h = header.str();
    client.send(h.c_str(), h.length());
    
    // Send file data in chunks
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        client.send(buffer, file.gcount());
    }
    
    ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] File sent: %s (%zu bytes)\n"), 
              filename.c_str(), size));
}

// Handle incoming HTTP requests
void HTTPServer::HandleRequest(ACE_SOCK_Stream& client) {
    char buffer[4096];
    ssize_t bytes = client.recv(buffer, sizeof(buffer) - 1);
    if (bytes <= 0) return;
    
    buffer[bytes] = '\0';
    std::string request(buffer);
    
    // Parse request line
    size_t end = request.find("\r\n");
    if (end == std::string::npos) end = request.find("\n");
    if (end == std::string::npos) return;
    
    std::string line = request.substr(0, end);
    std::istringstream iss(line);
    std::string method, path, version;
    iss >> method >> path >> version;
    
    // Get remote address for logging
    ACE_INET_Addr remote_addr;
    std::string remote_host = "unknown";
    if (client.get_remote_addr(remote_addr) == 0) {
        remote_host = remote_addr.get_host_addr();
    }
    
    ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] %s %s from %s\n"), 
              method.c_str(), path.c_str(), remote_host.c_str()));
    
    // Check if path starts with /qr (handles /qr, /qr.png, /qr?anything)
    if (path.find("/qr") == 0) {
        ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] Matched QR endpoint!\n")));
        std::string qr_data = GenerateQRData();
        ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] Generating QR code: %s\n"), qr_data.c_str()));
        
        auto png = GenerateQRCodePNG(qr_data, 400);
        if (png.empty()) {
            SendErrorResponse(client, 500, "QR code generation failed");
            return;
        }
        
        std::ostringstream header;
        header << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: image/png\r\n"
               << "Content-Length: " << png.size() << "\r\n"
               << "Cache-Control: no-cache\r\n"
               << "Access-Control-Allow-Origin: *\r\n"
               << "\r\n";
        
        std::string h = header.str();
        client.send(h.c_str(), h.length());
        client.send(png.data(), png.size());
        
        ACE_DEBUG((LM_INFO, ACE_TEXT("[HTTPServer] QR code sent: %zu bytes\n"), png.size()));
        return;
    }
    
    // Handle file sharing endpoint
    if (path.find("/share/") == 0 && path.length() > 7) {
        // Extract token (strip query params if any)
        std::string token_part = path.substr(7);
        size_t q = token_part.find('?');
        std::string token = (q != std::string::npos) ? token_part.substr(0, q) : token_part;
        
        std::string file_path;
        {
            std::lock_guard<std::mutex> lock(token_mutex);
            auto it = share_tokens.find(token);
            if (it != share_tokens.end()) {
                file_path = it->second;
            }
        }
        
        if (file_path.empty()) {
            SendErrorResponse(client, 404, "Token not found or expired");
            return;
        }
        
        SendFileResponse(client, file_path);
        return;
    }
    
    // Handle connection info endpoint (JSON)
    if (path.find("/info") == 0) {
        std::ostringstream json;
        json << "{\n"
             << "  \"pc_id\": \"" << g_pc_id << "\",\n"
             << "  \"username\": \"" << g_username << "\",\n"
             << "  \"relay_server\": \"" << g_relay_server << "\",\n"
             << "  \"relay_port\": " << g_relay_port << ",\n"
             << "  \"auth_token\": \"" << g_auth_token << "\"\n"
             << "}";
        
        std::string body = json.str();
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: application/json\r\n"
                 << "Content-Length: " << body.length() << "\r\n"
                 << "Access-Control-Allow-Origin: *\r\n"
                 << "\r\n"
                 << body;
        
        std::string resp = response.str();
        client.send(resp.c_str(), resp.length());
        return;
    }
    
    // 404 Not Found
    SendErrorResponse(client, 404, "Path not found: " + path);
}

// Display QR code in terminal (ASCII art)
void DisplayQRCodeInTerminal(const std::string& data) {
    QRcode* qr = QRcode_encodeString(data.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    if (!qr) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("[HTTPServer] Failed to generate QR code for terminal\n")));
        return;
    }
    
    int width = qr->width;
    
    // Print top border
    std::cout << "\n";
    for (int i = 0; i < width + 4; ++i) std::cout << "█";
    std::cout << "\n";
    
    // Print QR code with 2-pixel border
    for (int y = 0; y < width; ++y) {
        std::cout << "██"; // Left border
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            if (qr->data[idx] & 1) {
                std::cout << "  "; // Black module (space)
            } else {
                std::cout << "██"; // White module (block)
            }
        }
        std::cout << "██\n"; // Right border
    }
    
    // Print bottom border
    for (int i = 0; i < width + 4; ++i) std::cout << "█";
    std::cout << "\n\n";
    
    QRcode_free(qr);
}

} // namespace Common
} // namespace RemoteAccessSystem

// C-style API for external access
extern "C" {
    void http_server_add_share_token(const char* token, const char* path) {
        RemoteAccessSystem::Common::AddShareToken(token, path);
    }
    
    void http_server_set_pc_info(const char* id, const char* user,
                                 const char* server, uint16_t port) {
        RemoteAccessSystem::Common::SetPCInfo(id, user, server, port);
    }
    
    const char* http_server_get_qr_data() {
        static std::string qr_data;
        std::ostringstream data;
        data << RemoteAccessSystem::Common::g_pc_id << "|" 
             << RemoteAccessSystem::Common::g_username << "|" 
             << RemoteAccessSystem::Common::g_relay_server << "|" 
             << RemoteAccessSystem::Common::g_relay_port << "|"
             << RemoteAccessSystem::Common::g_auth_token;
        qr_data = data.str();
        return qr_data.c_str();
    }
}