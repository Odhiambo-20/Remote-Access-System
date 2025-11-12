#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <atomic>
#include <fcntl.h>
#include <chrono>

std::atomic<bool> running(true);

struct PCInfo {
    std::string pc_id;
    std::string usb_id;
    std::string username;
    int main_connection;
    int file_connection;
    time_t last_heartbeat;
};

struct PendingRequest {
    int mobile_client;
    std::string request_type;
    std::string pc_id;
    std::string file_path;
    size_t file_size;
    size_t bytes_transferred;
    time_t timestamp;
};

std::map<std::string, PCInfo> connected_pcs;
std::map<int, PendingRequest> pending_requests;  // mobile_socket -> request info
std::mutex pc_mutex;
std::mutex request_mutex;

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        parts.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    
    parts.push_back(str.substr(start));
    return parts;
}

void handleDownloadDataTransfer(int pc_fd, int mobile_fd, size_t file_size) {
    std::cout << "[RelayServer] Starting download data transfer: " << file_size << " bytes" << std::endl;
    
    char buffer[8192];
    size_t total_transferred = 0;
    
    while (total_transferred < file_size && running) {
        size_t remaining = file_size - total_transferred;
        size_t to_read = std::min(remaining, sizeof(buffer));
        
        ssize_t bytes_read = recv(pc_fd, buffer, to_read, 0);
        
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                std::cout << "[RelayServer] PC connection closed during download" << std::endl;
            } else {
                std::cout << "[RelayServer] Error reading from PC: " << strerror(errno) << std::endl;
            }
            break;
        }
        
        ssize_t bytes_sent = send(mobile_fd, buffer, bytes_read, 0);
        
        if (bytes_sent <= 0) {
            if (bytes_sent == 0) {
                std::cout << "[RelayServer] Mobile connection closed during download" << std::endl;
            } else {
                std::cout << "[RelayServer] Error sending to mobile: " << strerror(errno) << std::endl;
            }
            break;
        }
        
        total_transferred += bytes_sent;
        
        if (total_transferred % 102400 == 0 || total_transferred == file_size) {
            int progress = (total_transferred * 100) / file_size;
            std::cout << "[RelayServer] Download progress: " << progress << "% (" 
                      << total_transferred << "/" << file_size << " bytes)" << std::endl;
        }
    }
    
    std::cout << "[RelayServer] Download data transfer complete: " << total_transferred << " bytes" << std::endl;
}

void handleUploadDataTransfer(int mobile_fd, int pc_fd, size_t file_size) {
    std::cout << "[RelayServer] Starting upload data transfer: " << file_size << " bytes" << std::endl;
    
    char buffer[8192];
    size_t total_transferred = 0;
    
    while (total_transferred < file_size && running) {
        size_t remaining = file_size - total_transferred;
        size_t to_read = std::min(remaining, sizeof(buffer));
        
        ssize_t bytes_read = recv(mobile_fd, buffer, to_read, 0);
        
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                std::cout << "[RelayServer] Mobile connection closed during upload" << std::endl;
            } else {
                std::cout << "[RelayServer] Error reading from mobile: " << strerror(errno) << std::endl;
            }
            break;
        }
        
        ssize_t bytes_sent = send(pc_fd, buffer, bytes_read, 0);
        
        if (bytes_sent <= 0) {
            if (bytes_sent == 0) {
                std::cout << "[RelayServer] PC connection closed during upload" << std::endl;
            } else {
                std::cout << "[RelayServer] Error sending to PC: " << strerror(errno) << std::endl;
            }
            break;
        }
        
        total_transferred += bytes_sent;
        
        if (total_transferred % 102400 == 0 || total_transferred == file_size) {
            int progress = (total_transferred * 100) / file_size;
            std::cout << "[RelayServer] Upload progress: " << progress << "% (" 
                      << total_transferred << "/" << file_size << " bytes)" << std::endl;
        }
    }
    
    std::cout << "[RelayServer] Upload data transfer complete: " << total_transferred << " bytes" << std::endl;
}

void handleFileConnection(int client_fd, const std::string& pc_id) {
    std::cout << "[RelayServer] FileHandler connected for PC: " << pc_id << std::endl;
    
    // Send acknowledgment
    std::string response = "OK|FILE_HANDLER_REGISTERED\n";
    send(client_fd, response.c_str(), response.length(), 0);
    
    // Keep connection open and handle file requests/responses
    char buffer[8192];
    std::string accumulated_data;
    
    while (running) {
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
        
        if (bytes > 0) {
            buffer[bytes] = '\0';
            accumulated_data += std::string(buffer);
            
            // Process complete messages (ending with \n)
            size_t pos;
            while ((pos = accumulated_data.find('\n')) != std::string::npos) {
                std::string message = accumulated_data.substr(0, pos);
                accumulated_data.erase(0, pos + 1);
                
                if (message.empty()) continue;
                
                std::cout << "[FileHandler-" << pc_id << "] Received: " << message << std::endl;
                
                // Handle responses from PC FileHandler
                if (message.find("DIR_LIST|") == 0) {
                    std::lock_guard<std::mutex> lock(request_mutex);
                    for (auto it = pending_requests.begin(); it != pending_requests.end(); ++it) {
                        if (it->second.pc_id == pc_id && it->second.request_type == "LIST_DIR") {
                            std::cout << "[RelayServer] Forwarding DIR_LIST to mobile client fd=" << it->first << std::endl;
                            std::string forward_msg = message + "\n";
                            send(it->first, forward_msg.c_str(), forward_msg.length(), 0);
                            close(it->first);
                            pending_requests.erase(it);
                            break;
                        }
                    }
                }
                else if (message.find("DOWNLOAD_START|") == 0) {
                    // Format: DOWNLOAD_START|pc_id|file_path|file_size
                    auto parts = split(message, '|');
                    if (parts.size() >= 4) {
                        size_t file_size = std::stoull(parts[3]);
                        
                        std::lock_guard<std::mutex> lock(request_mutex);
                        for (auto it = pending_requests.begin(); it != pending_requests.end(); ++it) {
                            if (it->second.pc_id == pc_id && it->second.request_type == "DOWNLOAD") {
                                int mobile_fd = it->first;
                                std::cout << "[RelayServer] Starting download relay for " << file_size << " bytes" << std::endl;
                                
                                // Send DOWNLOAD_START to mobile
                                std::string start_msg = message + "\n";
                                send(mobile_fd, start_msg.c_str(), start_msg.length(), 0);
                                
                                // Transfer file data from PC to mobile
                                handleDownloadDataTransfer(client_fd, mobile_fd, file_size);
                                
                                close(mobile_fd);
                                pending_requests.erase(it);
                                break;
                            }
                        }
                    }
                }
                else if (message.find("SHARE_URL|") == 0) {
                    std::lock_guard<std::mutex> lock(request_mutex);
                    for (auto it = pending_requests.begin(); it != pending_requests.end(); ++it) {
                        if (it->second.pc_id == pc_id && it->second.request_type == "GENERATE_URL") {
                            std::cout << "[RelayServer] Forwarding SHARE_URL to mobile client fd=" << it->first << std::endl;
                            std::string forward_msg = message + "\n";
                            send(it->first, forward_msg.c_str(), forward_msg.length(), 0);
                            close(it->first);
                            pending_requests.erase(it);
                            break;
                        }
                    }
                }
                else if (message.find("ERROR|") == 0) {
                    std::cout << "[RelayServer] Error received from PC: " << message << std::endl;
                    
                    std::lock_guard<std::mutex> lock(request_mutex);
                    for (auto it = pending_requests.begin(); it != pending_requests.end(); ++it) {
                        if (it->second.pc_id == pc_id) {
                            std::cout << "[RelayServer] Forwarding error to mobile client fd=" << it->first 
                                      << " (request_type=" << it->second.request_type << ")" << std::endl;
                            std::string forward_msg = message + "\n";
                            send(it->first, forward_msg.c_str(), forward_msg.length(), 0);
                            close(it->first);
                            pending_requests.erase(it);
                            break;
                        }
                    }
                }
                else if (message.find("UPLOAD_READY") == 0) {
                    std::cout << "[RelayServer] PC ready for upload" << std::endl;
                    
                    std::lock_guard<std::mutex> lock(request_mutex);
                    for (auto it = pending_requests.begin(); it != pending_requests.end(); ++it) {
                        if (it->second.pc_id == pc_id && it->second.request_type == "UPLOAD") {
                            int mobile_fd = it->first;
                            size_t file_size = it->second.file_size;
                            
                            std::cout << "[RelayServer] Found pending upload for mobile fd=" << mobile_fd 
                                      << ", file_size=" << file_size << std::endl;
                            
                            // Send UPLOAD_READY to mobile
                            std::string ready_msg = "UPLOAD_READY\n";
                            ssize_t sent = send(mobile_fd, ready_msg.c_str(), ready_msg.length(), 0);
                            
                            if (sent <= 0) {
                                std::cout << "[RelayServer] Failed to send UPLOAD_READY to mobile" << std::endl;
                                close(mobile_fd);
                                pending_requests.erase(it);
                                break;
                            }
                            
                            std::cout << "[RelayServer] Sent UPLOAD_READY to mobile, starting file data relay..." << std::endl;
                            
                            // Transfer file data from mobile to PC
                            handleUploadDataTransfer(mobile_fd, client_fd, file_size);
                            
                            std::cout << "[RelayServer] File data relay complete, waiting for PC confirmation..." << std::endl;
                            break;
                        }
                    }
                }
                else if (message.find("UPLOAD_COMPLETE") == 0 || message.find("UPLOAD_SUCCESS") == 0) {
                    std::cout << "[RelayServer] Upload completed, notifying mobile" << std::endl;
                    
                    std::lock_guard<std::mutex> lock(request_mutex);
                    for (auto it = pending_requests.begin(); it != pending_requests.end(); ++it) {
                        if (it->second.pc_id == pc_id && it->second.request_type == "UPLOAD") {
                            std::cout << "[RelayServer] Sending success notification to mobile fd=" << it->first << std::endl;
                            std::string forward_msg = message + "\n";
                            send(it->first, forward_msg.c_str(), forward_msg.length(), 0);
                            close(it->first);
                            pending_requests.erase(it);
                            break;
                        }
                    }
                }
                else if (message.find("HEARTBEAT") == 0) {
                    std::string pong = "PONG\n";
                    send(client_fd, pong.c_str(), pong.length(), 0);
                }
            }
        }
        else if (bytes == 0) {
            std::cout << "[FileHandler] Connection closed for PC: " << pc_id << std::endl;
            break;
        }
        else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cout << "[FileHandler] Error on connection for PC: " << pc_id << " - " << strerror(errno) << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Remove file connection from PC info
    {
        std::lock_guard<std::mutex> lock(pc_mutex);
        auto it = connected_pcs.find(pc_id);
        if (it != connected_pcs.end()) {
            it->second.file_connection = -1;
        }
    }
    
    close(client_fd);
    std::cout << "[FileHandler] Handler thread exiting for PC: " << pc_id << std::endl;
}

void handleUploadRequest(int client_fd, const std::string& pc_id, const std::string& file_path, size_t file_size) {
    std::cout << "[RelayServer] Handling upload in dedicated thread for fd=" << client_fd << std::endl;
    
    // Find PC file handler
    int pc_file_fd = -1;
    {
        std::lock_guard<std::mutex> lock(pc_mutex);
        auto it = connected_pcs.find(pc_id);
        if (it != connected_pcs.end() && it->second.file_connection != -1) {
            pc_file_fd = it->second.file_connection;
        }
    }
    
    if (pc_file_fd == -1) {
        std::cout << "[RelayServer] PC file handler not connected" << std::endl;
        std::string error = "ERROR|PC file handler not connected\n";
        send(client_fd, error.c_str(), error.length(), 0);
        close(client_fd);
        return;
    }
    
    // Store pending upload request BEFORE forwarding to PC
    {
        std::lock_guard<std::mutex> req_lock(request_mutex);
        PendingRequest req;
        req.mobile_client = client_fd;
        req.request_type = "UPLOAD";
        req.pc_id = pc_id;
        req.file_path = file_path;
        req.file_size = file_size;
        req.bytes_transferred = 0;
        req.timestamp = time(nullptr);
        pending_requests[client_fd] = req;
        std::cout << "[RelayServer] Stored pending upload request for mobile fd=" << client_fd << std::endl;
    }
    
    // Forward UPLOAD command to PC FileHandler
    std::string forward_cmd = "UPLOAD|" + pc_id + "|" + file_path + "|" + std::to_string(file_size) + "\n";
    ssize_t sent = send(pc_file_fd, forward_cmd.c_str(), forward_cmd.length(), 0);
    
    if (sent <= 0) {
        std::cout << "[RelayServer] Failed to forward UPLOAD to PC" << std::endl;
        std::string error = "ERROR|Failed to contact PC\n";
        send(client_fd, error.c_str(), error.length(), 0);
        
        std::lock_guard<std::mutex> req_lock(request_mutex);
        pending_requests.erase(client_fd);
        close(client_fd);
        return;
    }
    
    std::cout << "[RelayServer] Forwarded UPLOAD command to PC FileHandler, keeping mobile socket open" << std::endl;
}

void handleDownloadRequest(int client_fd, const std::string& pc_id, const std::string& file_path) {
    std::cout << "[RelayServer] Handling download request for fd=" << client_fd << std::endl;
    
    // Find PC file handler
    int pc_file_fd = -1;
    {
        std::lock_guard<std::mutex> lock(pc_mutex);
        auto it = connected_pcs.find(pc_id);
        if (it != connected_pcs.end() && it->second.file_connection != -1) {
            pc_file_fd = it->second.file_connection;
        }
    }
    
    if (pc_file_fd == -1) {
        std::cout << "[RelayServer] PC file handler not connected" << std::endl;
        std::string error = "ERROR|PC file handler not connected\n";
        send(client_fd, error.c_str(), error.length(), 0);
        close(client_fd);
        return;
    }
    
    // Store pending download request
    {
        std::lock_guard<std::mutex> req_lock(request_mutex);
        PendingRequest req;
        req.mobile_client = client_fd;
        req.request_type = "DOWNLOAD";
        req.pc_id = pc_id;
        req.file_path = file_path;
        req.file_size = 0;
        req.bytes_transferred = 0;
        req.timestamp = time(nullptr);
        pending_requests[client_fd] = req;
        std::cout << "[RelayServer] Stored pending download request for mobile fd=" << client_fd << std::endl;
    }
    
    // Forward DOWNLOAD command to PC FileHandler
    std::string forward_cmd = "DOWNLOAD|" + pc_id + "|" + file_path + "\n";
    ssize_t sent = send(pc_file_fd, forward_cmd.c_str(), forward_cmd.length(), 0);
    
    if (sent <= 0) {
        std::cout << "[RelayServer] Failed to forward DOWNLOAD to PC" << std::endl;
        std::string error = "ERROR|Failed to contact PC\n";
        send(client_fd, error.c_str(), error.length(), 0);
        
        std::lock_guard<std::mutex> req_lock(request_mutex);
        pending_requests.erase(client_fd);
        close(client_fd);
        return;
    }
    
    std::cout << "[RelayServer] Forwarded DOWNLOAD command to PC FileHandler, keeping mobile socket open" << std::endl;
}

void handleClient(int client_fd) {
    char buffer[1024];
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::string message(buffer);
        if (!message.empty() && message.back() == '\n') message.pop_back();
        
        std::cout << "[RelayServer] Received from fd=" << client_fd << ": " << message << std::endl;
        
        if (message.find("REGISTER|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 4) {
                std::string pc_id = parts[1];
                PCInfo info;
                info.pc_id = pc_id;
                info.usb_id = parts[2];
                info.username = parts[3];
                info.main_connection = client_fd;
                info.file_connection = -1;
                info.last_heartbeat = time(nullptr);
                
                {
                    std::lock_guard<std::mutex> lock(pc_mutex);
                    auto it = connected_pcs.find(pc_id);
                    if (it != connected_pcs.end()) {
                        info.file_connection = it->second.file_connection;
                    }
                    connected_pcs[pc_id] = info;
                }
                
                std::string response = "OK|REGISTERED\n";
                send(client_fd, response.c_str(), response.length(), 0);
                std::cout << "[RelayServer] PC registered: " << pc_id << " (" << parts[3] << ")" << std::endl;
                return;
            } else {
                send(client_fd, "ERROR|Invalid REGISTER format\n", 33, 0);
                close(client_fd);
            }
        }
        else if (message.find("FILE_HANDLER_REGISTER|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 2) {
                std::string pc_id = parts[1];
                
                {
                    std::lock_guard<std::mutex> lock(pc_mutex);
                    auto it = connected_pcs.find(pc_id);
                    if (it != connected_pcs.end()) {
                        if (it->second.file_connection != -1) {
                            close(it->second.file_connection);
                        }
                        it->second.file_connection = client_fd;
                    } else {
                        PCInfo info;
                        info.pc_id = pc_id;
                        info.main_connection = -1;
                        info.file_connection = client_fd;
                        info.last_heartbeat = time(nullptr);
                        connected_pcs[pc_id] = info;
                    }
                }
                
                std::cout << "[RelayServer] FileHandler registered for PC: " << pc_id << std::endl;
                std::thread(&handleFileConnection, client_fd, pc_id).detach();
                return;
            } else {
                send(client_fd, "ERROR|Invalid FILE_HANDLER_REGISTER format\n", 45, 0);
                close(client_fd);
            }
        }
        else if (message.find("PC_FILE|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 2) {
                std::string pc_id = parts[1];
                
                {
                    std::lock_guard<std::mutex> lock(pc_mutex);
                    auto it = connected_pcs.find(pc_id);
                    if (it != connected_pcs.end()) {
                        if (it->second.file_connection != -1) {
                            close(it->second.file_connection);
                        }
                        it->second.file_connection = client_fd;
                    } else {
                        PCInfo info;
                        info.pc_id = pc_id;
                        info.main_connection = -1;
                        info.file_connection = client_fd;
                        info.last_heartbeat = time(nullptr);
                        connected_pcs[pc_id] = info;
                    }
                }
                
                std::cout << "[RelayServer] FileHandler registering for PC: " << pc_id << std::endl;
                std::thread(&handleFileConnection, client_fd, pc_id).detach();
                return;
            } else {
                send(client_fd, "ERROR|Invalid PC_FILE format\n", 31, 0);
                close(client_fd);
            }
        }
        else if (message.find("LIST_DIR|") == 0) {
            // Format: LIST_DIR|pc_id|path
            auto parts = split(message, '|');
            if (parts.size() >= 3) {
                std::string pc_id = parts[1];
                std::string path = parts[2];
                
                std::lock_guard<std::mutex> lock(pc_mutex);
                auto it = connected_pcs.find(pc_id);
                if (it != connected_pcs.end() && it->second.file_connection != -1) {
                    // Store pending request
                    {
                        std::lock_guard<std::mutex> req_lock(request_mutex);
                        PendingRequest req;
                        req.mobile_client = client_fd;
                        req.request_type = "LIST_DIR";
                        req.pc_id = pc_id;
                        req.timestamp = time(nullptr);
                        pending_requests[client_fd] = req;
                    }
                    
                    // Forward to PC FileHandler
                    std::string forward_cmd = "LIST_DIR|" + pc_id + "|" + path + "\n";
                    send(it->second.file_connection, forward_cmd.c_str(), forward_cmd.length(), 0);
                    std::cout << "[RelayServer] Forwarded LIST_DIR to PC FileHandler" << std::endl;
                    return; // Keep socket open for response
                } else {
                    std::string error = "ERROR|PC file handler not connected\n";
                    send(client_fd, error.c_str(), error.length(), 0);
                    close(client_fd);
                }
            } else {
                send(client_fd, "ERROR|Invalid LIST_DIR format\n", 32, 0);
                close(client_fd);
            }
        }
        else if (message.find("DOWNLOAD|") == 0) {
            // Format: DOWNLOAD|pc_id|file_path
            auto parts = split(message, '|');
            if (parts.size() >= 3) {
                std::string pc_id = parts[1];
                std::string file_path = parts[2];
                
                std::cout << "[RelayServer] DOWNLOAD request: " << file_path << std::endl;
                
                // Handle download in dedicated thread to keep socket open
                std::thread(&handleDownloadRequest, client_fd, pc_id, file_path).detach();
                return; // Socket will be managed by download thread
            } else {
                send(client_fd, "ERROR|Invalid DOWNLOAD format\n", 32, 0);
                close(client_fd);
            }
        }
        else if (message.find("UPLOAD|") == 0) {
            // Format: UPLOAD|pc_id|file_path|file_size
            auto parts = split(message, '|');
            if (parts.size() >= 4) {
                std::string pc_id = parts[1];
                std::string file_path = parts[2];
                size_t file_size = std::stoull(parts[3]);
                
                std::cout << "[RelayServer] UPLOAD request: " << file_path << " (" << file_size << " bytes)" << std::endl;
                
                // Handle upload in dedicated thread to keep socket open
                std::thread(&handleUploadRequest, client_fd, pc_id, file_path, file_size).detach();
                return; // Socket will be managed by upload thread
            } else {
                send(client_fd, "ERROR|Invalid UPLOAD format\n", 30, 0);
                close(client_fd);
            }
        }
        else if (message.find("GENERATE_URL|") == 0) {
            // Format: GENERATE_URL|pc_id|file_path
            auto parts = split(message, '|');
            if (parts.size() >= 3) {
                std::string pc_id = parts[1];
                std::string file_path = parts[2];
                
                std::lock_guard<std::mutex> lock(pc_mutex);
                auto it = connected_pcs.find(pc_id);
                if (it != connected_pcs.end() && it->second.file_connection != -1) {
                    {
                        std::lock_guard<std::mutex> req_lock(request_mutex);
                        PendingRequest req;
                        req.mobile_client = client_fd;
                        req.request_type = "GENERATE_URL";
                        req.pc_id = pc_id;
                        req.timestamp = time(nullptr);
                        pending_requests[client_fd] = req;
                    }
                    
                    std::string forward_cmd = "GENERATE_URL|" + pc_id + "|" + file_path + "\n";
                    send(it->second.file_connection, forward_cmd.c_str(), forward_cmd.length(), 0);
                    std::cout << "[RelayServer] Forwarded GENERATE_URL to PC FileHandler" << std::endl;
                    return;
                } else {
                    std::string error = "ERROR|PC file handler not connected\n";
                    send(client_fd, error.c_str(), error.length(), 0);
                    close(client_fd);
                }
            } else {
                send(client_fd, "ERROR|Invalid GENERATE_URL format\n", 36, 0);
                close(client_fd);
            }
        }
        else if (message.find("GET_PCS") == 0) {
            std::string response = "PC_LIST|";
            {
                std::lock_guard<std::mutex> lock(pc_mutex);
                for (const auto& pc : connected_pcs) {
                    if (pc.second.main_connection != -1) {
                        response += pc.second.pc_id + "," + pc.second.username + "," + pc.second.pc_id + ";";
                    }
                }
            }
            response += "\n";
            send(client_fd, response.c_str(), response.length(), 0);
            close(client_fd);
        }
        else {
            std::cout << "[RelayServer] Unknown command: " << message << std::endl;
            close(client_fd);
        }
    } else {
        close(client_fd);
    }
}

void cleanupStaleRequests() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        
        std::lock_guard<std::mutex> lock(request_mutex);
        time_t now = time(nullptr);
        
        for (auto it = pending_requests.begin(); it != pending_requests.end(); ) {
            if (now - it->second.timestamp > 300) { // 5 minutes timeout
                std::cout << "[RelayServer] Cleaning up stale request: type=" 
                          << it->second.request_type << ", fd=" << it->first << std::endl;
                close(it->first);
                it = pending_requests.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void signalHandler(int signal) {
    std::cout << "\n[RelayServer] Shutting down..." << std::endl;
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Relay Server v2.0" << std::endl;
    std::cout << "  Enhanced File Transfer Support" << std::endl;
    std::cout << "========================================" << std::endl;
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "[RelayServer] Failed to create socket: " << strerror(errno) << std::endl;
        return 1;
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "[RelayServer] setsockopt failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(2810);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[RelayServer] Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    
    if (listen(server_fd, 10) < 0) {
        std::cerr << "[RelayServer] Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    
    std::cout << "[RelayServer] Listening on port 2810" << std::endl;
    std::cout << "[RelayServer] Waiting for connections..." << std::endl;
    
    // Start cleanup thread
    std::thread cleanup_thread(cleanupStaleRequests);
    cleanup_thread.detach();
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            if (running && errno != EINTR) {
                std::cerr << "[RelayServer] Accept failed: " << strerror(errno) << std::endl;
            }
            continue;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        
        std::cout << "[RelayServer] New client connected from " << client_ip 
                  << ":" << ntohs(client_addr.sin_port) 
                  << ", fd=" << client_fd << std::endl;
        
        std::thread(&handleClient, client_fd).detach();
    }
    
    // Cleanup
    std::cout << "[RelayServer] Closing all connections..." << std::endl;
    
    {
        std::lock_guard<std::mutex> lock(pc_mutex);
        for (auto& pc : connected_pcs) {
            if (pc.second.main_connection != -1) {
                close(pc.second.main_connection);
            }
            if (pc.second.file_connection != -1) {
                close(pc.second.file_connection);
            }
        }
        connected_pcs.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(request_mutex);
        for (auto& req : pending_requests) {
            close(req.first);
        }
        pending_requests.clear();
    }
    
    close(server_fd);
    std::cout << "[RelayServer] Shutdown complete" << std::endl;
    return 0;
}