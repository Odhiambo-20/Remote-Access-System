#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>
#include "database_manager.h"
#include "../../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

class ServerManager {
public:
    ServerManager(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, DatabaseManager& db_manager);
    ~ServerManager();
    bool Start(const std::string& service_name, const std::string& endpoint);

private:
    CORBA::ORB_var orb_;
    PortableServer::POA_var poa_;
    DatabaseManager& db_manager_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // SERVER_MANAGER_H