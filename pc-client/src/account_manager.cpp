#include "account_manager.h"
#include "../../common/include/utils.h"
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

AccountManager::AccountManager(const std::string& config_file) 
    : socket_()
    , config_file_(config_file)
    , authenticated_(false)
    , current_user_("") {
    
    Utils::Log(LM_INFO, "AccountManager initialized with config: " + config_file);
    
    // Load configuration
    if (!LoadConfig(config_file)) {
        Utils::Log(LM_WARNING, "Failed to load configuration file: " + config_file);
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
    
    Utils::Log(LM_INFO, "AccountManager destroyed");
}

bool AccountManager::Authenticate(const std::string& username, const std::string& password) {
    Utils::Log(LM_INFO, "Attempting authentication for user: " + username);
    
    // Validate input
    if (username.empty() || password.empty()) {
        Utils::Log(LM_ERROR, "Username or password is empty");
        return false;
    }
    
    // Validate credentials format
    if (!ValidateCredentials(username, password)) {
        Utils::Log(LM_ERROR, "Invalid credentials format");
        return false;
    }
    
    // TODO: Implement actual authentication logic
    authenticated_ = true;
    current_user_ = username;
    
    Utils::Log(LM_INFO, "Authentication successful for user: " + username);
    return true;
}

bool AccountManager::Register(const std::string& username, const std::string& password) {
    Utils::Log(LM_INFO, "Attempting registration for user: " + username);
    
    // Validate input
    if (username.empty() || password.empty()) {
        Utils::Log(LM_ERROR, "Username or password is empty");
        return false;
    }
    
    // Validate credentials format
    if (!ValidateCredentials(username, password)) {
        Utils::Log(LM_ERROR, "Invalid credentials format for registration");
        return false;
    }
    
    Utils::Log(LM_INFO, "Registration successful for user: " + username);
    return true;
}

bool AccountManager::Logout() {
    if (!authenticated_) {
        Utils::Log(LM_WARNING, "Logout called but user not authenticated");
        return false;
    }
    
    Utils::Log(LM_INFO, "Logging out user: " + current_user_);
    
    authenticated_ = false;
    current_user_.clear();
    
    Utils::Log(LM_INFO, "Logout successful");
    return true;
}

bool AccountManager::LoadConfig(const std::string& config_file) {
    Utils::Log(LM_INFO, "Loading configuration from: " + config_file);
    // TODO: Implement actual config loading
    return true;
}

bool AccountManager::ValidateCredentials(const std::string& username, const std::string& password) {
    // Basic validation
    if (username.length() < 3 || username.length() > 32) {
        Utils::Log(LM_ERROR, "Username length must be between 3 and 32 characters");
        return false;
    }
    
    if (password.length() < 8 || password.length() > 128) {
        Utils::Log(LM_ERROR, "Password length must be between 8 and 128 characters");
        return false;
    }
    
    // Check for valid characters in username (alphanumeric and underscore)
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') {
            Utils::Log(LM_ERROR, "Username contains invalid characters");
            return false;
        }
    }
    
    return true;
}

} // namespace Common
} // namespace 