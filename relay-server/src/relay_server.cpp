#include "relay_server.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>
#include <ace/Log_Msg.h>
#include <thread>
#include <map>
#include <mutex>
#include <sstream>
#include <ctime>
#include <set>

namespace RemoteAccessSystem {
namespace Common {

struct PCInfo {
    std::string pc_id;
    std::string usb_id;
    std::string username;
    time_t last_heartbeat;
    ACE_HANDLE control_handle;  // Main connection
    ACE_HANDLE file_handle;     // File handler connection
};

static std::map<std::string, PCInfo> connected_pcs;
static std::mutex pc_mutex;

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Forward data between two sockets (for file transfer)
void BridgeConnection(ACE_HANDLE from_handle, ACE_HANDLE to_handle, bool* running) {
    char buffer[8192];
    while (*running) {
        ssize_t bytes = ACE_OS::recv(from_handle, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            break;
        }
        
        ssize_t sent = ACE_OS::send(to_handle, buffer, bytes, 0);
        if (sent <= 0) {
            break;
        }
    }
}

void HandleClient(ACE_HANDLE handle, bool* running) {
    char buffer[4096];
    std::string pc_id;
    bool registered = false;
    bool is_file_handler = false;
    
    while (*running) {
        ssize_t bytes = ACE_OS::recv(handle, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            if (registered) {
                std::lock_guard<std::mutex> lock(pc_mutex);
                if (is_file_handler && connected_pcs.count(pc_id)) {
                    connected_pcs[pc_id].file_handle = ACE_INVALID_HANDLE;
                } else {
                    connected_pcs.erase(pc_id);
                }
                ACE_DEBUG((LM_INFO, "PC %s disconnected (%s)\n", 
                          pc_id.c_str(), is_file_handler ? "file handler" : "control"));
            }
            break;
        }
        
        buffer[bytes] = '\0';
        std::string message(buffer);
        message = trim(message);
        
        ACE_DEBUG((LM_DEBUG, "Received: %s\n", message.c_str()));
        
        // Handle REGISTER command from PC client
        if (message.find("REGISTER|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 4) {
                pc_id = parts[1];
                PCInfo info;
                info.pc_id = parts[1];
                info.usb_id = parts[2];
                info.username = parts[3];
                info.last_heartbeat = time(nullptr);
                info.control_handle = handle;
                info.file_handle = ACE_INVALID_HANDLE;
                
                std::lock_guard<std::mutex> lock(pc_mutex);
                connected_pcs[pc_id] = info;
                registered = true;
                
                std::string response = "OK|REGISTERED\n";
                ACE_OS::send(handle, response.c_str(), response.length(), 0);
                ACE_DEBUG((LM_INFO, "PC registered: %s (%s)\n", pc_id.c_str(), info.username.c_str()));
            }
        }
        // Handle FILE_HANDLER_REGISTER from PC client's file handler
        else if (message.find("FILE_HANDLER_REGISTER|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 2) {
                pc_id = parts[1];
                std::lock_guard<std::mutex> lock(pc_mutex);
                if (connected_pcs.count(pc_id)) {
                    connected_pcs[pc_id].file_handle = handle;
                    registered = true;
                    is_file_handler = true;
                    
                    std::string response = "OK|FILE_HANDLER_REGISTERED\n";
                    ACE_OS::send(handle, response.c_str(), response.length(), 0);
                    ACE_DEBUG((LM_INFO, "File handler registered for PC: %s\n", pc_id.c_str()));
                } else {
                    std::string response = "ERROR|PC not registered\n";
                    ACE_OS::send(handle, response.c_str(), response.length(), 0);
                    break;
                }
            }
        }
        // Handle HEARTBEAT
        else if (message.find("HEARTBEAT|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 2) {
                std::lock_guard<std::mutex> lock(pc_mutex);
                if (connected_pcs.count(parts[1])) {
                    connected_pcs[parts[1]].last_heartbeat = time(nullptr);
                    std::string response = "OK\n";
                    ACE_OS::send(handle, response.c_str(), response.length(), 0);
                }
            }
        }
        // Handle PING from file handler
        else if (message.find("PING") == 0) {
            std::string response = "PONG\n";
            ACE_OS::send(handle, response.c_str(), response.length(), 0);
        }
        // Handle GET_PCS from mobile app
        else if (message.find("GET_PCS") == 0) {
            std::lock_guard<std::mutex> lock(pc_mutex);
            std::string response = "PC_LIST|";
            time_t now = time(nullptr);
            for (const auto& pair : connected_pcs) {
                if (now - pair.second.last_heartbeat < 120) {
                    response += pair.second.pc_id + "," + pair.second.usb_id + "," + pair.second.username + ";";
                }
            }
            response += "\n";
            ACE_OS::send(handle, response.c_str(), response.length(), 0);
            ACE_DEBUG((LM_INFO, "Sent PC list to mobile app\n"));
            break;
        }
        // Handle LIST_DIR from mobile app
        else if (message.find("LIST_DIR|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 3) {
                std::string target_pc = parts[1];
                std::string path = parts[2];
                
                std::lock_guard<std::mutex> lock(pc_mutex);
                if (connected_pcs.count(target_pc) && 
                    connected_pcs[target_pc].file_handle != ACE_INVALID_HANDLE) {
                    
                    ACE_HANDLE pc_handle = connected_pcs[target_pc].file_handle;
                    
                    // Forward command to PC's file handler
                    std::string forward_msg = message + "\n";
                    ACE_OS::send(pc_handle, forward_msg.c_str(), forward_msg.length(), 0);
                    ACE_DEBUG((LM_INFO, "Forwarded LIST_DIR to PC %s\n", target_pc.c_str()));
                    
                    // Wait for response from PC and forward back to mobile
                    char pc_buffer[8192];
                    ssize_t pc_bytes = ACE_OS::recv(pc_handle, pc_buffer, sizeof(pc_buffer) - 1, 0);
                    if (pc_bytes > 0) {
                        pc_buffer[pc_bytes] = '\0';
                        ACE_OS::send(handle, pc_buffer, pc_bytes, 0);
                        ACE_DEBUG((LM_INFO, "Forwarded response back to mobile\n"));
                    }
                } else {
                    std::string response = "ERROR|PC not connected or file handler not available\n";
                    ACE_OS::send(handle, response.c_str(), response.length(), 0);
                }
            }
            break;
        }
        // Handle UPLOAD from mobile app
        else if (message.find("UPLOAD|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 4) {
                std::string target_pc = parts[1];
                std::string remote_path = parts[2];
                long long file_size = std::stoll(parts[3]);
                
                ACE_DEBUG((LM_INFO, "Upload request: PC=%s, path=%s, size=%lld\n", 
                          target_pc.c_str(), remote_path.c_str(), file_size));
                
                std::lock_guard<std::mutex> lock(pc_mutex);
                if (connected_pcs.count(target_pc) && 
                    connected_pcs[target_pc].file_handle != ACE_INVALID_HANDLE) {
                    
                    ACE_HANDLE pc_handle = connected_pcs[target_pc].file_handle;
                    
                    // Forward UPLOAD command to PC's file handler
                    std::string forward_msg = message + "\n";
                    ACE_OS::send(pc_handle, forward_msg.c_str(), forward_msg.length(), 0);
                    
                    // Wait for UPLOAD_READY response from PC
                    char pc_buffer[256];
                    ssize_t pc_bytes = ACE_OS::recv(pc_handle, pc_buffer, sizeof(pc_buffer) - 1, 0);
                    if (pc_bytes > 0) {
                        pc_buffer[pc_bytes] = '\0';
                        std::string pc_response = trim(std::string(pc_buffer));
                        
                        // Send response to mobile
                        ACE_OS::send(handle, pc_buffer, pc_bytes, 0);
                        
                        if (pc_response.find("UPLOAD_READY") == 0) {
                            ACE_DEBUG((LM_INFO, "PC ready, bridging file transfer\n"));
                            
                            // Bridge the connection for file data transfer
                            long long transferred = 0;
                            char file_buffer[8192];
                            
                            while (transferred < file_size && *running) {
                                ssize_t to_read = std::min((long long)sizeof(file_buffer), 
                                                          file_size - transferred);
                                ssize_t bytes_read = ACE_OS::recv(handle, file_buffer, to_read, 0);
                                if (bytes_read <= 0) {
                                    ACE_DEBUG((LM_ERROR, "Upload failed: connection lost\n"));
                                    break;
                                }
                                
                                ssize_t bytes_sent = ACE_OS::send(pc_handle, file_buffer, bytes_read, 0);
                                if (bytes_sent <= 0) {
                                    ACE_DEBUG((LM_ERROR, "Upload failed: cannot send to PC\n"));
                                    break;
                                }
                                
                                transferred += bytes_read;
                                
                                if (transferred % 1048576 == 0) { // Log every 1MB
                                    ACE_DEBUG((LM_INFO, "Upload progress: %lld/%lld bytes\n", 
                                              transferred, file_size));
                                }
                            }
                            
                            ACE_DEBUG((LM_INFO, "Upload complete: %lld bytes transferred\n", transferred));
                            
                            // Wait for final confirmation from PC
                            pc_bytes = ACE_OS::recv(pc_handle, pc_buffer, sizeof(pc_buffer) - 1, 0);
                            if (pc_bytes > 0) {
                                pc_buffer[pc_bytes] = '\0';
                                ACE_OS::send(handle, pc_buffer, pc_bytes, 0);
                                ACE_DEBUG((LM_INFO, "Upload status: %s\n", pc_buffer));
                            }
                        } else {
                            ACE_DEBUG((LM_ERROR, "PC not ready for upload: %s\n", pc_response.c_str()));
                        }
                    }
                } else {
                    std::string response = "ERROR|PC not connected or file handler not available\n";
                    ACE_OS::send(handle, response.c_str(), response.length(), 0);
                    ACE_DEBUG((LM_ERROR, "Upload failed: PC %s not available\n", target_pc.c_str()));
                }
            }
            break;
        }
        // Handle DOWNLOAD from mobile app
        else if (message.find("DOWNLOAD|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 3) {
                std::string target_pc = parts[1];
                std::string remote_path = parts[2];
                
                std::lock_guard<std::mutex> lock(pc_mutex);
                if (connected_pcs.count(target_pc) && 
                    connected_pcs[target_pc].file_handle != ACE_INVALID_HANDLE) {
                    
                    ACE_HANDLE pc_handle = connected_pcs[target_pc].file_handle;
                    
                    // Forward command to PC
                    std::string forward_msg = message + "\n";
                    ACE_OS::send(pc_handle, forward_msg.c_str(), forward_msg.length(), 0);
                    
                    // Bridge the connection for download
                    char bridge_buffer[8192];
                    while (*running) {
                        ssize_t bytes = ACE_OS::recv(pc_handle, bridge_buffer, sizeof(bridge_buffer), 0);
                        if (bytes <= 0) break;
                        
                        ssize_t sent = ACE_OS::send(handle, bridge_buffer, bytes, 0);
                        if (sent <= 0) break;
                    }
                } else {
                    std::string response = "ERROR|PC not connected\n";
                    ACE_OS::send(handle, response.c_str(), response.length(), 0);
                }
            }
            break;
        }
        // Handle GENERATE_URL from mobile app
        else if (message.find("GENERATE_URL|") == 0) {
            auto parts = split(message, '|');
            if (parts.size() >= 3) {
                std::string target_pc = parts[1];
                std::string file_path = parts[2];
                
                std::lock_guard<std::mutex> lock(pc_mutex);
                if (connected_pcs.count(target_pc) && 
                    connected_pcs[target_pc].file_handle != ACE_INVALID_HANDLE) {
                    
                    ACE_HANDLE pc_handle = connected_pcs[target_pc].file_handle;
                    
                    // Forward command to PC's file handler
                    std::string forward_msg = message + "\n";
                    ACE_OS::send(pc_handle, forward_msg.c_str(), forward_msg.length(), 0);
                    
                    // Wait for response and forward back
                    char pc_buffer[1024];
                    ssize_t pc_bytes = ACE_OS::recv(pc_handle, pc_buffer, sizeof(pc_buffer) - 1, 0);
                    if (pc_bytes > 0) {
                        pc_buffer[pc_bytes] = '\0';
                        ACE_OS::send(handle, pc_buffer, pc_bytes, 0);
                    }
                } else {
                    std::string response = "ERROR|PC not connected\n";
                    ACE_OS::send(handle, response.c_str(), response.length(), 0);
                }
            }
            break;
        }
    }
    
    ACE_OS::closesocket(handle);
}

RelayServer::RelayServer(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)), running_(false) {}

RelayServer::~RelayServer() {
    Stop();
}

bool RelayServer::Start(const std::string& address, uint16_t port) {
    ACE_INET_Addr addr(port, address.c_str());
    if (acceptor_.open(addr) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "Failed to open relay server\n"), false);
    }
    
    running_ = true;
    ACE_DEBUG((LM_INFO, "Relay server started on %s:%d\n", address.c_str(), port));
    
    while (running_) {
        ACE_SOCK_Stream client;
        ACE_Time_Value timeout(1);
        
        if (acceptor_.accept(client, nullptr, &timeout) != -1) {
            ACE_HANDLE handle = client.get_handle();
            std::thread([this, handle]() {
                HandleClient(handle, &running_);
            }).detach();
        }
    }
    return true;
}

void RelayServer::Stop() {
    running_ = false;
    acceptor_.close();
}

} // namespace Common
} // namespace RemoteAccessSystem