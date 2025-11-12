#include "relay_manager.h"
#include <iostream>
#include <thread>
#include <map>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <atomic>
#include <sstream>
#include <vector>

using namespace RemoteAccessSystem::RelayServer;

std::atomic<bool> running(true);
RelayManager* relay_manager = nullptr;

// Store file handler connections
std::map<std::string, int> pc_file_handlers;
std::mutex file_handler_mutex;

void signalHandler(int signal) {
    std::cout << "\n[RelayServer] Shutting down..." << std::endl;
    running = false;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

void handleClient(int client_fd, RelayManager* manager) {
    char buffer[1024];
    
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::string data(buffer);
        std::cout << "[RelayServer] Received: " << data << std::endl;
        
        if (data.find("REGISTER|") == 0) {
            // Handle PC registration
            auto parts = split(data, '|');
            if (parts.size() >= 4) {
                std::string pc_id = parts[1];
                std::string usb_id = parts[2];
                std::string username = parts[3];
                
                // Fixed: registerPC now expects 5 parameters
                // (pc_id, pc_name, username, relay_address, relay_port)
                manager->registerPC(pc_id, username, usb_id, "127.0.0.1", 2810);
                
                std::string response = "OK|REGISTERED\n";
                send(client_fd, response.c_str(), response.length(), 0);
                std::cout << "[RelayServer] PC registered: " << pc_id << " (" << username << ")" << std::endl;
                
                // Keep connection open for heartbeats
                while (running) {
                    bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0) break;
                    
                    buffer[bytes] = '\0';
                    std::string msg(buffer);
                    
                    if (msg.find("HEARTBEAT|") == 0) {
                        manager->updateHeartbeat(pc_id);
                        std::string hb_response = "OK\n";
                        send(client_fd, hb_response.c_str(), hb_response.length(), 0);
                    }
                }
                
                manager->unregisterPC(pc_id);
                std::cout << "[RelayServer] PC unregistered: " << pc_id << std::endl;
            }
        }
        else if (data.find("PC_FILE|") == 0) {
            // Handle file handler registration
            auto parts = split(data, '|');
            if (parts.size() >= 2) {
                std::string pc_id = parts[1];
                
                {
                    std::lock_guard<std::mutex> lock(file_handler_mutex);
                    pc_file_handlers[pc_id] = client_fd;
                }
                
                std::string response = "OK|FILE_REGISTERED\n";
                send(client_fd, response.c_str(), response.length(), 0);
                std::cout << "[RelayServer] File handler registered for PC: " << pc_id << std::endl;
                
                // Keep connection open for file operations
                while (running) {
                    bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0) break;
                    
                    buffer[bytes] = '\0';
                    std::string msg(buffer);
                    std::cout << "[RelayServer] File handler message: " << msg << std::endl;
                }
                
                {
                    std::lock_guard<std::mutex> lock(file_handler_mutex);
                    pc_file_handlers.erase(pc_id);
                }
                std::cout << "[RelayServer] File handler disconnected for PC: " << pc_id << std::endl;
                return; // Don't close socket here, already handled
            }
        }
        else if (data.find("GET_PCS") == 0) {
            auto pc_list = manager->getPCList();
            std::string response = "PC_LIST|";
            
            for (const auto& pc : pc_list) {
                response += pc.pc_id + "," + pc.username + "," + pc.pc_name + ";";
            }
            response += "\n";
            
            send(client_fd, response.c_str(), response.length(), 0);
        }
    }
    
    close(client_fd);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Relay Server v1.0" << std::endl;
    std::cout << "========================================" << std::endl;
    
    relay_manager = new RelayManager();
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "[RelayServer] Failed to create socket" << std::endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(2810);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[RelayServer] Bind failed" << std::endl;
        close(server_fd);
        return 1;
    }
    
    if (listen(server_fd, 5) < 0) {
        std::cerr << "[RelayServer] Listen failed" << std::endl;
        close(server_fd);
        return 1;
    }
    
    std::cout << "[RelayServer] Listening on port 2810" << std::endl;
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (running) continue;
            else break;
        }
        
        std::cout << "[RelayServer] New client connected" << std::endl;
        std::thread(&handleClient, client_fd, relay_manager).detach();
    }
    
    close(server_fd);
    delete relay_manager;
    
    std::cout << "[RelayServer] Shutdown complete" << std::endl;
    return 0;
}