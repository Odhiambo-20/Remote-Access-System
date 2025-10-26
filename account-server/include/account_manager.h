#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <map>
#include <mutex>

namespace RemoteAccessSystem {
namespace AccountServer {

struct UserAccount {
    std::string username;
    std::string password_hash;
    std::string email;
    std::string session_token;
    bool is_active;
};

class AccountManager {
public:
    AccountManager();
    
    bool authenticate(const std::string& username, const std::string& password);
    bool createAccount(const std::string& username, const std::string& password, const std::string& email);
    bool deleteAccount(const std::string& username);
    std::string generateSessionToken(const std::string& username);
    bool validateToken(const std::string& token);
    
private:
    std::string hashPassword(const std::string& password);
    void initializeDefaultAccounts();
    
private:
    std::map<std::string, UserAccount> accounts_;
    std::mutex mutex_;
};

} // namespace AccountServer
} // namespace RemoteAccessSystem

#endif // ACCOUNT_MANAGER_H
