#include "relay_manager.h"
#include <iostream>
#include <chrono>

namespace RemoteAccessSystem {
namespace RelayServer {

RelayManager::RelayManager() {
    std::cout << "[RelayManager] Initialized" << std::endl;
}

bool RelayManager::registerPC(const std::string& pc_id, const std::string& pc_name,
                               const std::string& username, const std::string& address, int port) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PCClient pc;
    pc.pc_id = pc_id;
    pc.pc_name = pc_name;
    pc.username = username;
    pc.address = address;
    pc.port = port;
    pc.last_heartbeat = std::chrono::system_clock::now().time_since_epoch().count();
    pc.is_online = true;
    
    registered_pcs_[pc_id] = pc;
    std::cout << "[RelayManager] Registered PC: " << pc_name << " (" << pc_id << ")" << std::endl;
    
    return true;
}

bool RelayManager::unregisterPC(const std::string& pc_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = registered_pcs_.find(pc_id);
    if (it == registered_pcs_.end()) {
        return false;
    }
    
    registered_pcs_.erase(it);
    std::cout << "[RelayManager] Unregistered PC: " << pc_id << std::endl;
    return true;
}

std::vector<PCClient> RelayManager::getPCList() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PCClient> list;
    for (const auto& pair : registered_pcs_) {
        if (pair.second.is_online) {
            list.push_back(pair.second);
        }
    }
    
    return list;
}

bool RelayManager::updateHeartbeat(const std::string& pc_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = registered_pcs_.find(pc_id);
    if (it == registered_pcs_.end()) {
        return false;
    }
    
    it->second.last_heartbeat = std::chrono::system_clock::now().time_since_epoch().count();
    it->second.is_online = true;
    
    return true;
}

PCClient RelayManager::getPCInfo(const std::string& pc_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = registered_pcs_.find(pc_id);
    if (it != registered_pcs_.end()) {
        return it->second;
    }
    
    return PCClient{};
}

void RelayManager::cleanupOfflinePC() {
    // Cleanup PCs that haven't sent heartbeat for 5 minutes
    long timeout = 300000000000; // 5 minutes in nanoseconds
    long current_time = std::chrono::system_clock::now().time_since_epoch().count();
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = registered_pcs_.begin();
    while (it != registered_pcs_.end()) {
        if (current_time - it->second.last_heartbeat > timeout) {
            std::cout << "[RelayManager] Removing offline PC: " << it->first << std::endl;
            it = registered_pcs_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace RelayServer
} // namespace RemoteAccessSystem
