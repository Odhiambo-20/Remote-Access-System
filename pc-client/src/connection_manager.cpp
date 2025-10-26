#include "connection_manager.h"
#include <ace/INET_Addr.h>
#include "../../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

ConnectionManager::ConnectionManager() {}

bool ConnectionManager::ConnectToRelay(const std::string& relay_address) {
    try {
        ACE_INET_Addr addr(relay_address.c_str());
        // Connection logic here
        Utils::Log(ACE_Log_Priority::LM_INFO, "Connected to relay: " + relay_address);
        return true;
    } catch (const std::exception& e) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to connect to relay: " + std::string(e.what()));
        return false;
    }
}

} // namespace Common
} // namespace RemoteAccessSystem