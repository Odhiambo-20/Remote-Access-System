#ifndef RELAY_MANAGER_H
#define RELAY_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <memory>

namespace RemoteAccessSystem {
namespace RelayServer {

struct PCClient {
    std::string pc_id;
    std::string pc_name;
    std::string username;
    std::string address;
    int port;
    int socket_fd;
    long last_heartbeat;
    bool is_online;
};

class RelayManager {
public:
    RelayManager();
    
    bool registerPC(const std::string& pc_id, const std::string& pc_name, 
                    const std::string& username, const std::string& address, int port);
    bool unregisterPC(const std::string& pc_id);
    std::vector<PCClient> getPCList();
    bool updateHeartbeat(const std::string& pc_id);
    PCClient getPCInfo(const std::string& pc_id);
    
private:
    void cleanupOfflinePC();
    
private:
    std::map<std::string, PCClient> registered_pcs_;
    std::mutex mutex_;
};

} // namespace RelayServer
} // namespace RemoteAccessSystem

#endif // RELAY_MANAGER_H
