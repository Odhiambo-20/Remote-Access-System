#include "server_manager.h"
#include "account_server_impl.h"
#include "../../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

ServerManager::ServerManager(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, DatabaseManager& db_manager)
    : orb_(CORBA::ORB::_duplicate(orb))
    , poa_(PortableServer::POA::_duplicate(poa))
    , db_manager_(db_manager) {
    Utils::Log(ACE_Log_Priority::LM_INFO, "ServerManager initialized");
}

bool ServerManager::Start(const std::string& service_name, const std::string& endpoint) {
    try {
        PortableServer::POAManager_var poa_manager = poa_->the_POAManager();
        
        // Create the servant implementation with db_manager reference
        AccountServerImpl* impl = new AccountServerImpl(db_manager_);
        
        // Activate the servant
        PortableServer::ServantBase_var servant = impl;
        PortableServer::ObjectId_var id = poa_->activate_object(servant.in());
        
        CORBA::Object_var obj = poa_->id_to_reference(id.in());
        CORBA::String_var ior = orb_->object_to_string(obj.in());
        
        Utils::Log(ACE_Log_Priority::LM_INFO, "Account Server IOR: " + std::string(ior.in()));
        
        poa_manager->activate();
        
        Utils::Log(ACE_Log_Priority::LM_INFO, "Account Server started on endpoint: " + endpoint);
        
        return true;
    } catch (const CORBA::Exception& e) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "CORBA exception in Start: " + std::string(e._name()));
        return false;
    }
}

ServerManager::~ServerManager() {
    if (!CORBA::is_nil(orb_.in())) {
        try {
            orb_->shutdown(false);
        } catch (...) {
            // Ignore exceptions during shutdown
        }
    }
    Utils::Log(ACE_Log_Priority::LM_INFO, "ServerManager destroyed");
}

} // namespace Common
} // namespace RemoteAccessSystem
