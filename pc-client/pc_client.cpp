#include "pc_client.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_unistd.h>
#include <sstream>

PCClient::PCClient(const std::string& pc_id, const std::string& username, const std::string& hostname)
    : pc_id_(pc_id)
    , username_(username)
    , hostname_(hostname)
    , running_(false) {
}

PCClient::~PCClient() {
    shutdown();
}

int PCClient::connect_to_account_server(const std::string& server_address) {
    std::istringstream iss(server_address);
    std::string host;
    int port;
    char delimiter;
    
    std::getline(iss, host, ':');
    iss >> port;
    
    ACE_INET_Addr server_addr(port, host.c_str());
    ACE_SOCK_Connector connector;
    
    if (connector.connect(account_stream_, server_addr) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "[PCClient] Failed to connect to account server\n"), -1);
    }
    
    ACE_DEBUG((LM_INFO, "[PCClient] Connected to account server\n"));
    return 0;
}

int PCClient::register_with_relay(const std::string& relay_address, int port) {
    ACE_INET_Addr server_addr(port, relay_address.c_str());
    ACE_SOCK_Connector connector;
    
    if (connector.connect(relay_stream_, server_addr) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "[PCClient] Failed to connect to relay server\n"), -1);
    }
    
    ACE_DEBUG((LM_INFO, "[PCClient] Connected to relay server\n"));
    
    std::ostringstream registration;
    registration << "REGISTER|" << pc_id_ << "|usb_device|" << username_ << "\n";
    
    std::string reg_str = registration.str();
    ssize_t bytes_sent = relay_stream_.send(reg_str.c_str(), reg_str.length());
    
    if (bytes_sent != (ssize_t)reg_str.length()) {
        ACE_ERROR_RETURN((LM_ERROR, "[PCClient] Failed to send registration\n"), -1);
    }
    
    ACE_DEBUG((LM_INFO, "[PCClient] Sent registration: %s", reg_str.c_str()));
    
    char buffer[256];
    ssize_t bytes = relay_stream_.recv(buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        ACE_DEBUG((LM_INFO, "[PCClient] Relay response: %s", buffer));
        
        std::string response(buffer);
        if (response.find("OK|REGISTERED") != std::string::npos) {
            ACE_DEBUG((LM_INFO, "[PCClient] Registration successful!\n"));
            running_ = true;
            return this->activate(THR_NEW_LWP | THR_JOINABLE);
        } else {
            ACE_ERROR_RETURN((LM_ERROR, "[PCClient] Registration failed: %s\n", buffer), -1);
        }
    } else {
        ACE_ERROR_RETURN((LM_ERROR, "[PCClient] No response from relay server\n"), -1);
    }
    
    return -1;
}

int PCClient::svc() {
    ACE_DEBUG((LM_INFO, "[PCClient] Heartbeat thread started\n"));
    
    while (running_) {
        ACE_OS::sleep(30);
        
        if (!running_) break;
        
        std::ostringstream heartbeat;
        heartbeat << "HEARTBEAT|" << pc_id_ << "\n";
        
        std::string hb_str = heartbeat.str();
        ssize_t sent = relay_stream_.send(hb_str.c_str(), hb_str.length());
        
        if (sent == -1) {
            ACE_ERROR((LM_ERROR, "[PCClient] Failed to send heartbeat\n"));
            break;
        }
    }
    
    ACE_DEBUG((LM_INFO, "[PCClient] Heartbeat thread stopped\n"));
    return 0;
}

void PCClient::shutdown() {
    running_ = false;
    
    // Wait for thread to finish
    this->wait();
    
    // Close connections
    if (account_stream_.get_handle() != ACE_INVALID_HANDLE) {
        account_stream_.close();
    }
    if (relay_stream_.get_handle() != ACE_INVALID_HANDLE) {
        relay_stream_.close();
    }
}
