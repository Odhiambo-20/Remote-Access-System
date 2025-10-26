#ifndef ACCOUNT_SERVER_IMPL_H
#define ACCOUNT_SERVER_IMPL_H

#include <tao/PortableServer/PortableServer.h>
#include <tao/PortableServer/Servant_Base.h>
#include <memory>
#include <string>
#include <vector>
#include "../../common/include/types.h"
#include "database_manager.h"

namespace RemoteAccessSystem {
namespace Common {

class AccountServerImpl : public virtual PortableServer::ServantBase {
public:
    explicit AccountServerImpl(DatabaseManager& db_manager);
    virtual ~AccountServerImpl() = default;
    
    // CORBA required virtual functions
    virtual void _dispatch(TAO_ServerRequest& request, TAO::Portable_Server::Servant_Upcall* upcall);
    virtual const char* _interface_repository_id() const;
    
    // Business logic functions
    bool initialize(const std::string& db_path);
    void shutdown();
    bool registerUser(const std::string& username, const std::string& password, const std::string& email);
    bool authenticateUser(const std::string& username, const std::string& password);
    bool changePassword(const std::string& username, const std::string& old_password, const std::string& new_password, const std::string& email);
    bool deleteUser(const std::string& username);
    bool userExists(const std::string& username);
    std::string createSession(const std::string& username, const std::string& device_id, const std::string& device_type);
    bool validateSession(const std::string& session_token);
    Session getSession(const std::string& session_token);
    bool updateSessionActivity(const std::string& session_token);
    bool destroySession(const std::string& session_token);
    void cleanupExpiredSessions();
    bool registerPC(const PCRegistration& registration);
    bool updatePCStatus(const std::string& pc_id, bool online);
    bool updatePCInfo(const PCRegistration& registration);
    PCInfo getPCInfo(const std::string& pc_id);
    std::vector<PCInfo> getUserPCs(const std::string& username);
    std::vector<PCInfo> getOnlinePCs(const std::string& username);
    bool deletePC(const std::string& pc_id);
    bool pcExists(const std::string& pc_id);
    void updatePCHeartbeat(const std::string& pc_id);
    void markOfflinePCs(uint64_t timeout_seconds);
    bool registerMobile(const MobileRegistration& registration);
    bool updateMobileInfo(const MobileRegistration& registration);
    bool deleteMobile(const std::string& device_id);
    bool mobileExists(const std::string& device_id);
    void updateMobileHeartbeat(const std::string& device_id);
    BindingResponse bindDevice(const BindingRequest& request);
    BindingResponse unbindDevice(const std::string& device_id, const std::string& pc_id);
    std::vector<PCInfo> getBoundPCs(const std::string& device_id);
    std::vector<std::string> getBoundMobileDevices(const std::string& pc_id);
    bool isDeviceBound(const std::string& device_id, const std::string& pc_id);
    int getBindingCount(const std::string& device_id);
    bool unbindAllDevices(const std::string& device_id);
    bool unbindAllPCs(const std::string& pc_id);
    ServerStats getServerStats();
    void incrementRequestCounter();
    void setSessionTimeout(uint64_t seconds);
    void setMaxBindingsPerDevice(int max);
    void setHeartbeatTimeout(uint64_t seconds);
    bool validateUsername(const std::string& username);
    bool validateDeviceId(const std::string& device_id);
    bool validatePCId(const std::string& pc_id);
    bool validateSessionToken(const std::string& token);
    std::string generateSessionToken();
    bool isSessionExpired(const Session& session);
    void loadActiveSessions();
    void saveSession(const Session& session);
    void removeSession(const std::string& session_token);
    void updatePCCache(const PCRegistration& registration);
    void removePCFromCache(const std::string& pc_id);
    void startMaintenanceThread();
    void stopMaintenanceThread();
    void maintenanceTask();

private:
    DatabaseManager& db_manager_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // ACCOUNT_SERVER_IMPL_H
