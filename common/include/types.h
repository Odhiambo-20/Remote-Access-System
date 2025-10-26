#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

namespace RemoteAccessSystem {
namespace Common {

struct Session {
    std::string token;
    std::string username;
    std::string device_id;
    uint64_t created_at;
    uint64_t last_activity;
};

struct PCRegistration {
    std::string pc_id;
    std::string username;
    std::string name;
};

struct MobileRegistration {
    std::string device_id;
    std::string username;
    std::string name;
};

struct BindingRequest {
    std::string device_id;
    std::string pc_id;
};

struct BindingResponse {
    bool success;
    std::string message;
};

struct PCInfo {
    std::string pc_id;
    std::string username;
    std::string name;
    bool online;
    uint64_t last_heartbeat;
};

struct AccountServerConfig {
    std::string database_path;
    uint64_t session_timeout;
    int max_bindings_per_device;
    uint64_t heartbeat_timeout;

    static AccountServerConfig loadFromFile(const std::string& config_file);
    static AccountServerConfig defaultConfig();
};

struct ServerStats {
    int active_sessions;
    int total_requests;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // TYPES_H
