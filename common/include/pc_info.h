#ifndef PC_INFO_H
#define PC_INFO_H

#include <string>
#include <cstdint>

namespace RemoteAccessSystem::Common {

struct PCInfo {
    std::string pc_id;
    std::string pc_name;
    std::string username;
    bool is_online;
    bool online;  // Alias for compatibility
    uint64_t last_seen;
    std::string ip_address;
    std::string ipv4_address;
    std::string ipv6_address;
    std::string relay_endpoint;
};

} // namespace RemoteAccessSystem::Common

#endif // PC_INFO_H
