#include "database_manager.h"
#include <sqlite3.h>
#include <sstream>
#include <sys/stat.h>
#include "../../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

ResultSet::ResultSet(sqlite3_stmt* stmt) : stmt_(stmt) {}

std::string ResultSet::getString(int column) {
    const unsigned char* text = sqlite3_column_text(stmt_, column);
    return std::string(reinterpret_cast<const char*>(text));
}

ResultSet::~ResultSet() {
    sqlite3_finalize(stmt_);
}

DatabaseManager::DatabaseManager(const std::string& db_path) : db_(nullptr) {
    initialize(db_path);
}

bool DatabaseManager::initialize(const std::string& db_path) {
    // Check if file exists using stat
    struct stat buffer;
    bool db_exists = (stat(db_path.c_str(), &buffer) == 0);
    
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to open database: " + std::string(sqlite3_errmsg(db_)));
        return false;
    }
    return true;
}

void DatabaseManager::setSchemaVersion(int version) {
    std::stringstream ss;
    ss << "PRAGMA user_version = " << version;
    executeInternal(ss.str());
}

std::vector<std::string> DatabaseManager::getAllUsernames() {
    std::vector<std::string> usernames;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, "SELECT username FROM users", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            usernames.push_back(ResultSet(stmt).getString(0));
        }
        sqlite3_finalize(stmt);
    }
    return usernames;
}

bool DatabaseManager::saveSession(const std::string& token, const std::string& username, const std::string& device_id, uint64_t created_at) {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, "INSERT INTO sessions (token, username, device_id, created_at) VALUES (?, ?, ?, ?)", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, token.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, device_id.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 4, static_cast<int64_t>(created_at));
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

bool DatabaseManager::deleteExpiredSessions(uint64_t expiry_time) {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, "DELETE FROM sessions WHERE created_at < ?", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, static_cast<int64_t>(expiry_time));
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

std::vector<std::string> DatabaseManager::getUserMobileDevices(const std::string& username) {
    std::vector<std::string> devices;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, "SELECT device_id FROM devices WHERE username = ?", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            devices.push_back(ResultSet(stmt).getString(0));
        }
        sqlite3_finalize(stmt);
    }
    return devices;
}

std::vector<std::string> DatabaseManager::getBoundMobileDevices(const std::string& pc_id) {
    std::vector<std::string> devices;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, "SELECT device_id FROM bindings WHERE pc_id = ?", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, pc_id.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            devices.push_back(ResultSet(stmt).getString(0));
        }
        sqlite3_finalize(stmt);
    }
    return devices;
}

std::string DatabaseManager::getLastError() const {
    return std::string(sqlite3_errmsg(db_));
}

DatabaseManager::~DatabaseManager() {
    if (db_) sqlite3_close(db_);
}


// Internal execute method (used by setSchemaVersion)

// Internal execute method implementation
bool DatabaseManager::executeInternal(const std::string& query) {
    if (!db_) {
        return false;
    }
    
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        if (err_msg) {
            sqlite3_free(err_msg);
        }
        return false;
    }
    
    return true;
}

} // namespace Common
} // namespace RemoteAccessSystem
