#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <ace/ACE.h>
#include <ace/SOCK_Stream.h>

namespace RemoteAccessSystem {
namespace Common {

class AccountManager {
public:
    explicit AccountManager(const std::string& config_file);
    ~AccountManager();
    
    bool Authenticate(const std::string& username, const std::string& password);
    bool Register(const std::string& username, const std::string& password);
    bool Logout();
    
    // Getters
    bool IsAuthenticated() const { return authenticated_; }
    std::string GetCurrentUser() const { return current_user_; }

private:
    ACE_SOCK_Stream socket_;
    std::string config_file_;
    bool authenticated_;
    std::string current_user_;
    
    // Helper methods
    bool LoadConfig(const std::string& config_file);
    bool ValidateCredentials(const std::string& username, const std::string& password);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // ACCOUNT_MANAGER_H