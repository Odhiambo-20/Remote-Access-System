#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <string>
#include <ace/ACE.h>

namespace RemoteAccessSystem {
namespace Common {

class ConnectionManager {
public:
    ConnectionManager();
    bool ConnectToRelay(const std::string& relay_address);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // CONNECTION_MANAGER_H
