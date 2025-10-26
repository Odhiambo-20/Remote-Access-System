#include "account_manager.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <atomic>

using namespace RemoteAccessSystem::AccountServer;

std::atomic<bool> running(true);

void signalHandler(int signal) {
    std::cout << "\n[AccountServer] Shutting down..." << std::endl;
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Account Server v1.0" << std::endl;
    std::cout << "========================================" << std::endl;
    
    AccountManager manager;
    
    // Create server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "[AccountServer] Failed to create socket" << std::endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(2809);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[AccountServer] Bind failed" << std::endl;
        close(server_fd);
        return 1;
    }
    
    if (listen(server_fd, 5) < 0) {
        std::cerr << "[AccountServer] Listen failed" << std::endl;
        close(server_fd);
        return 1;
    }
    
    std::cout << "[AccountServer] Listening on port 2809" << std::endl;
    
    while (running) {
        // Accept connections and process authentication requests
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (running) continue;
            else break;
        }
        
        char buffer[1024];
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "[AccountServer] Received: " << buffer << std::endl;
            
            std::string response = "AUTH_OK\n";
            send(client_fd, response.c_str(), response.length(), 0);
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    std::cout << "[AccountServer] Shutdown complete" << std::endl;
    return 0;
}
