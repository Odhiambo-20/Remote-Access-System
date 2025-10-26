#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "../../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

class ResultSet {
public:
    explicit ResultSet(sqlite3_stmt* stmt);
    std::string getString(int column);
    ~ResultSet();

private:
    sqlite3_stmt* stmt_;
};

class DatabaseManager {
public:
    explicit DatabaseManager(const std::string& db_path);
    ~DatabaseManager();
    bool initialize(const std::string& db_path);
    void setSchemaVersion(int version);
    std::vector<std::string> getAllUsernames();
    bool saveSession(const std::string& token, const std::string& username, const std::string& device_id, uint64_t created_at);
    bool deleteExpiredSessions(uint64_t expiry_time);
    std::vector<std::string> getUserMobileDevices(const std::string& username);
    std::vector<std::string> getBoundMobileDevices(const std::string& pc_id);
    std::string getLastError() const;

private:
    bool executeInternal(const std::string& query);
    sqlite3* db_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // DATABASE_MANAGER_H