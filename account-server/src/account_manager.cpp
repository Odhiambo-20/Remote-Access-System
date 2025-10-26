#include "account_manager.h"
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <random>

namespace RemoteAccessSystem {
namespace AccountServer {

AccountManager::AccountManager() {
    initializeDefaultAccounts();
}

bool AccountManager::authenticate(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = accounts_.find(username);
    if (it == accounts_.end()) {
        std::cout << "[AccountManager] User not found: " << username << std::endl;
        return false;
    }
    
    std::string hashed = hashPassword(password);
    bool authenticated = (it->second.password_hash == hashed);
    
    std::cout << "[AccountManager] Authentication " 
              << (authenticated ? "SUCCESS" : "FAILED") 
              << " for user: " << username << std::endl;
    
    return authenticated;
}

bool AccountManager::createAccount(const std::string& username, const std::string& password, const std::string& email) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (accounts_.find(username) != accounts_.end()) {
        std::cout << "[AccountManager] User already exists: " << username << std::endl;
        return false;
    }
    
    UserAccount account;
    account.username = username;
    account.password_hash = hashPassword(password);
    account.email = email;
    account.is_active = true;
    
    accounts_[username] = account;
    std::cout << "[AccountManager] Account created: " << username << std::endl;
    return true;
}

bool AccountManager::deleteAccount(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = accounts_.find(username);
    if (it == accounts_.end()) {
        return false;
    }
    
    accounts_.erase(it);
    std::cout << "[AccountManager] Account deleted: " << username << std::endl;
    return true;
}

std::string AccountManager::generateSessionToken(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = accounts_.find(username);
    if (it == accounts_.end()) {
        return "";
    }
    
    // Generate random token
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        ss << std::hex << dis(gen);
    }
    
    it->second.session_token = ss.str();
    return it->second.session_token;
}

bool AccountManager::validateToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : accounts_) {
        if (pair.second.session_token == token && pair.second.is_active) {
            return true;
        }
    }
    
    return false;
}

std::string AccountManager::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), 
           password.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

void AccountManager::initializeDefaultAccounts() {
    // Create default test accounts
    createAccount("victor", "password123", "victor@example.com");
    createAccount("admin", "admin123", "admin@example.com");
    
    std::cout << "[AccountManager] Initialized with default accounts" << std::endl;
}

} // namespace AccountServer
} // namespace RemoteAccessSystem
