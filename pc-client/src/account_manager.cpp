#include "account_manager.h"
#include "../../common/include/utils.h"
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

AccountManager::AccountManager(const std::string& config_file) 
    : protocol_()
    , socket_()
    , config_file_(config_file)
    , authenticated_(false)
    , current_user_("") {
    
    Utils::Log(ACE_Log_Priority::LM_INFO, "AccountManager initialized with config: " + config_file);
    
    // Load configuration
    if (!LoadConfig(config_file)) {
        Utils::Log(ACE_Log_Priority::LM_WARNING, "Failed to load configuration file: " + config_file);
    }
}

AccountManager::~AccountManager() {
    if (authenticated_) {
        Logout();
    }
    
    // Close socket if open
    if (socket_.get_handle() != ACE_INVALID_HANDLE) {
        socket_.close();
    }
    
    Utils::Log(ACE_Log_Priority::LM_INFO, "AccountManager destroyed");
}

bool AccountManager::Authenticate(const std::string& username, const std::string& password) {
    Utils::Log(ACE_Log_Priority::LM_INFO, "Attempting authentication for user: " + username);
    
    // Validate input
    if (username.empty() || password.empty()) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Username or password is empty");
        return false;
    }
    
    // Validate credentials format
    if (!ValidateCredentials(username, password)) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Invalid credentials format");
        return false;
    }
    
    // TODO: Implement actual authentication logic
    // This would typically involve:
    // 1. Connecting to authentication server
    // 2. Sending encrypted credentials
    // 3. Receiving and validating authentication token
    
    // For now, return success and set authenticated state
    authenticated_ = true;
    current_user_ = username;
    
    Utils::Log(ACE_Log_Priority::LM_INFO, "Authentication successful for user: " + username);
    return true;
}

bool AccountManager::Register(const std::string& username, const std::string& password) {
    Utils::Log(ACE_Log_Priority::LM_INFO, "Attempting registration for user: " + username);
    
    // Validate input
    if (username.empty() || password.empty()) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Username or password is empty");
        return false;
    }
    
    // Validate credentials format
    if (!ValidateCredentials(username, password)) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Invalid credentials format for registration");
        return false;
    }
    
    // TODO: Implement actual registration logic
    // This would typically involve:
    // 1. Connecting to registration server
    // 2. Checking username availability
    // 3. Storing hashed credentials securely
    
    Utils::Log(ACE_Log_Priority::LM_INFO, "Registration successful for user: " + username);
    return true;
}

bool AccountManager::Logout() {
    if (!authenticated_) {
        Utils::Log(ACE_Log_Priority::LM_WARNING, "Logout called but user not authenticated");
        return false;
    }
    
    Utils::Log(ACE_Log_Priority::LM_INFO, "Logging out user: " + current_user_);
    
    // TODO: Implement logout logic
    // This would typically involve:
    // 1. Invalidating session token
    // 2. Notifying server of logout
    // 3. Clearing local session data
    
    authenticated_ = false;
    current_user_.clear();
    
    Utils::Log(ACE_Log_Priority::LM_INFO, "Logout successful");
    return true;
}

bool AccountManager::LoadConfig(const std::string& config_file) {
    // TODO: Implement configuration loading
    // Parse configuration file and load settings
    // For now, just return true
    Utils::Log(ACE_Log_Priority::LM_INFO, "Loading configuration from: " + config_file);
    return true;
}

bool AccountManager::ValidateCredentials(const std::string& username, const std::string& password) {
    // Basic validation
    if (username.length() < 3 || username.length() > 32) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Username length must be between 3 and 32 characters");
        return false;
    }
    
    if (password.length() < 8 || password.length() > 128) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Password length must be between 8 and 128 characters");
        return false;
    }
    
    // Check for valid characters in username (alphanumeric and underscore)
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Username contains invalid characters");
            return false;
        }
    }
    
    return true;
}

} // namespace Common
} // namespace RemoteAccessSystem