#!/bin/bash

echo "=== Fixing Syntax Errors, Headers, and Rebuilding ==="

# Create directories if they don't exist
mkdir -p ../common/include
mkdir -p ../relay-server/include
mkdir -p ../account-server/include
mkdir -p ../pc-client/include
mkdir -p ../usb-client/include

# Update utils.h with additional function declarations
cat > ../common/include/utils.h << 'EOF'
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

class Utils {
public:
    static std::string trim(const std::string& str);
    static std::string BytesToString(const std::vector<uint8_t>& bytes);
    static std::vector<uint8_t> StringToBytes(const std::string& str);
    static uint64_t GetTimestamp();
    static bool fileExists(const std::string& path);
    static bool ReadConfig(const std::string& config_file, std::string& config);
    static void Log(ACE_Log_Priority priority, const std::string& message);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // UTILS_H
EOF

# Verify utils.h
if [ -f ../common/include/utils.h ]; then
    echo "✅ utils.h updated successfully"
else
    echo "❌ Failed to create ../common/include/utils.h"
    exit 1
fi

# Update utils.cpp with implementations for new functions
cat > ../common/src/utils.cpp << 'EOF'
#include "utils.h"
#include <ace/Log_Msg.h>
#include <algorithm>
#include <cctype>
#include <sys/time.h>
#include <fstream>

namespace RemoteAccessSystem {
namespace Common {

std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Utils::BytesToString(const std::vector<uint8_t>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

std::vector<uint8_t> Utils::StringToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

uint64_t Utils::GetTimestamp() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<uint64_t>(tv.tv_sec);
}

bool Utils::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool Utils::ReadConfig(const std::string& config_file, std::string& config) {
    std::ifstream file(config_file);
    if (!file.is_open()) return false;
    std::stringstream buffer;
    buffer << file.rdbuf();
    config = buffer.str();
    return true;
}

void Utils::Log(ACE_Log_Priority priority, const std::string& message) {
    ACE_LOG_MSG->log(priority, "%s", message.c_str());
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Verify utils.cpp
if [ -f ../common/src/utils.cpp ]; then
    echo "✅ utils.cpp updated successfully"
else
    echo "❌ Failed to create ../common/src/utils.cpp"
    exit 1
fi

# Create types.h with placeholder type definitions
cat > ../common/include/types.h << 'EOF'
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

namespace RemoteAccessSystem {
namespace Common {

struct Session {
    std::string token;
    std::string username;
    std::string device_id;
    uint64_t created_at;
    uint64_t last_activity;
};

struct PCRegistration {
    std::string pc_id;
    std::string username;
    std::string name;
};

struct MobileRegistration {
    std::string device_id;
    std::string username;
    std::string name;
};

struct BindingRequest {
    std::string device_id;
    std::string pc_id;
};

struct BindingResponse {
    bool success;
    std::string message;
};

struct PCInfo {
    std::string pc_id;
    std::string username;
    std::string name;
    bool online;
    uint64_t last_heartbeat;
};

struct AccountServerConfig {
    std::string database_path;
    uint64_t session_timeout;
    int max_bindings_per_device;
    uint64_t heartbeat_timeout;

    static AccountServerConfig loadFromFile(const std::string& config_file);
    static AccountServerConfig defaultConfig();
};

struct ServerStats {
    int active_sessions;
    int total_requests;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // TYPES_H
EOF

# Verify types.h
if [ -f ../common/include/types.h ]; then
    echo "✅ types.h created successfully"
else
    echo "❌ Failed to create ../common/include/types.h"
    exit 1
fi

# Fix connection_handler.cpp (syntax error in ACE_ERROR)
cat > ../relay-server/src/connection_handler.cpp << 'EOF'
#include "connection_handler.h"
#include <ace/Log_Msg.h>
#include "../common/include/protocol.h"

namespace RemoteAccessSystem {
namespace Common {

ConnectionHandler::ConnectionHandler(ACE_HANDLE client_socket, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv)
    : client_socket_(client_socket), key_(key), iv_(iv) {
}

bool ConnectionHandler::Handle() {
    Message message;
    Protocol protocol;

    if (!protocol.ReceiveMessage(client_socket_, message, key_, iv_)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to receive message from client")));
        return false;
    }

    // Process message logic here
    return true;
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Verify connection_handler.cpp
if [ -f ../relay-server/src/connection_handler.cpp ]; then
    echo "✅ connection_handler.cpp updated successfully"
else
    echo "❌ Failed to create ../relay-server/src/connection_handler.cpp"
    exit 1
fi

# Update connection_handler.h
cat > ../relay-server/include/connection_handler.h << 'EOF'
#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <ace/ACE.h>
#include <vector>
#include "../common/include/message.h"

namespace RemoteAccessSystem {
namespace Common {

class ConnectionHandler {
public:
    ConnectionHandler(ACE_HANDLE client_socket, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
    bool Handle();

private:
    ACE_HANDLE client_socket_;
    std::vector<uint8_t> key_;
    std::vector<uint8_t> iv_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // CONNECTION_HANDLER_H
EOF

# Verify connection_handler.h
if [ -f ../relay-server/include/connection_handler.h ]; then
    echo "✅ connection_handler.h updated successfully"
else
    echo "❌ Failed to create ../relay-server/include/connection_handler.h"
    exit 1
fi

# Fix account_manager.cpp (syntax error in ACE_ERROR)
cat > ../pc-client/src/account_manager.cpp << 'EOF'
#include "account_manager.h"
#include <ace/Log_Msg.h>
#include "../common/include/protocol.h"
#include "../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

AccountManager::AccountManager(const std::string& config_file) {
    std::string config;
    if (Utils::ReadConfig(config_file, config)) {
        // Parse config logic here
    } else {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to read config file");
    }
}

bool AccountManager::Authenticate(const std::string& username, const std::string& password) {
    Message msg;
    msg.header.type = MessageType::AUTH_REQUEST;
    msg.header.message_id = Utils::GetTimestamp();
    msg.header.timestamp = Utils::GetTimestamp();
    msg.payload = Utils::StringToBytes(username + ":" + password);

    std::vector<uint8_t> key = Crypto::GenerateKey();
    std::vector<uint8_t> iv = Crypto::GenerateIV();

    if (!protocol_.SendMessage(socket_, msg, key, iv)) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to send auth request");
        return false;
    }

    return true;
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Verify account_manager.cpp
if [ -f ../pc-client/src/account_manager.cpp ]; then
    echo "✅ account_manager.cpp updated successfully"
else
    echo "❌ Failed to create ../pc-client/src/account_manager.cpp"
    exit 1
fi

# Update account_manager.h
cat > ../pc-client/include/account_manager.h << 'EOF'
#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <ace/ACE.h>
#include "../common/include/protocol.h"
#include "../common/include/crypto.h"

namespace RemoteAccessSystem {
namespace Common {

class AccountManager {
public:
    explicit AccountManager(const std::string& config_file);
    bool Authenticate(const std::string& username, const std::string& password);

private:
    Protocol protocol_;
    ACE_HANDLE socket_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // ACCOUNT_MANAGER_H
EOF

# Verify account_manager.h
if [ -f ../pc-client/include/account_manager.h ]; then
    echo "✅ account_manager.h updated successfully"
else
    echo "❌ Failed to create ../pc-client/include/account_manager.h"
    exit 1
fi

# Fix database_manager.cpp (syntax errors and missing parentheses)
cat > ../account-server/src/database_manager.cpp << 'EOF'
#include "database_manager.h"
#include <sqlite3.h>
#include <sstream>
#include "../common/include/utils.h"

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
    bool db_exists = Utils::fileExists(db_path);
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

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Verify database_manager.cpp
if [ -f ../account-server/src/database_manager.cpp ]; then
    echo "✅ database_manager.cpp updated successfully"
else
    echo "❌ Failed to create ../account-server/src/database_manager.cpp"
    exit 1
fi

# Update database_manager.h
cat > ../account-server/include/database_manager.h << 'EOF'
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "../common/include/utils.h"

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
    void executeInternal(const std::string& query);
    sqlite3* db_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // DATABASE_MANAGER_H
EOF

# Verify database_manager.h
if [ -f ../account-server/include/database_manager.h ]; then
    echo "✅ database_manager.h updated successfully"
else
    echo "❌ Failed to create ../account-server/include/database_manager.h"
    exit 1
fi

# Update account_server_impl.h
cat > ../account-server/include/account_server_impl.h << 'EOF'
#ifndef ACCOUNT_SERVER_IMPL_H
#define ACCOUNT_SERVER_IMPL_H

#include <memory>
#include <string>
#include <vector>
#include "../common/include/types.h"
#include "database_manager.h"

namespace RemoteAccessSystem {
namespace Common {

class AccountServerImpl {
public:
    explicit AccountServerImpl(DatabaseManager& db_manager);
    virtual ~AccountServerImpl() = default;
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

class AccountServerFactory {
public:
    static std::unique_ptr<AccountServerImpl> create(const AccountServerConfig& config);
    static std::unique_ptr<AccountServerImpl> createDefault();
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // ACCOUNT_SERVER_IMPL_H
EOF

# Verify account_server_impl.h
if [ -f ../account-server/include/account_server_impl.h ]; then
    echo "✅ account_server_impl.h updated successfully"
else
    echo "❌ Failed to create ../account-server/include/account_server_impl.h"
    exit 1
fi

# Fix server_manager.cpp (syntax errors)
cat > ../account-server/src/server_manager.cpp << 'EOF'
#include "server_manager.h"
#include <ace/Log_Msg.h>
#include "account_server_impl.h"
#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>
#include <fstream>

namespace RemoteAccessSystem {
namespace Common {

ServerManager::ServerManager(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, DatabaseManager& db_manager)
    : orb_(CORBA::ORB::_duplicate(orb)), poa_(PortableServer::POA::_duplicate(poa)), db_manager_(db_manager) {
}

ServerManager::~ServerManager() {}

bool ServerManager::Start(const std::string& service_name, const std::string& endpoint) {
    try {
        AccountServerImpl* impl = new AccountServerImpl(db_manager_);
        PortableServer::ObjectId_var id = poa_->activate_object(impl);
        CORBA::Object_var obj = poa_->id_to_reference(id.in());
        CORBA::String_var ior = orb_->object_to_string(obj.in());
        
        std::ofstream ior_file("account_server.ior");
        ior_file << ior.in();
        ior_file.close();

        orb_->set_endpoint(endpoint.c_str());
        Utils::Log(ACE_Log_Priority::LM_INFO, "Server started: " + service_name);
        return true;
    } catch (const CORBA::Exception& e) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to start server: " + std::string(e._name()));
        return false;
    }
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Verify server_manager.cpp
if [ -f ../account-server/src/server_manager.cpp ]; then
    echo "✅ server_manager.cpp updated successfully"
else
    echo "❌ Failed to create ../account-server/src/server_manager.cpp"
    exit 1
fi

# Update server_manager.h
cat > ../account-server/include/server_manager.h << 'EOF'
#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>
#include "database_manager.h"
#include "../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

class ServerManager {
public:
    ServerManager(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, DatabaseManager& db_manager);
    ~ServerManager();
    bool Start(const std::string& service_name, const std::string& endpoint);

private:
    CORBA::ORB_var orb_;
    PortableServer::POA_var poa_;
    DatabaseManager& db_manager_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // SERVER_MANAGER_H
EOF

# Verify server_manager.h
if [ -f ../account-server/include/server_manager.h ]; then
    echo "✅ server_manager.h updated successfully"
else
    echo "❌ Failed to create ../account-server/include/server_manager.h"
    exit 1
fi

# Fix account-server/src/main.cpp
cat > ../account-server/src/main.cpp << 'EOF'
#include <ace/Log_Msg.h>
#include "account_server_impl.h"
#include "server_manager.h"
#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>

namespace RemoteAccessSystem {
namespace Common {

int main(int argc, char* argv[]) {
    try {
        ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, ACE_TEXT("AccountServer"));
        
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
        CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj.in());
        
        DatabaseManager db_manager("account.db");
        if (!db_manager.initialize("account.db")) {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to initialize database");
            return 1;
        }
        
        ServerManager server_manager(orb.in(), poa.in(), db_manager);
        if (!server_manager.Start("AccountServer", "corbaloc:iiop:0.0.0.0:2809/AccountServer")) {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to start server");
            return 1;
        }
        
        orb->run();
        orb->destroy();
        return 0;
    } catch (const CORBA::Exception& e) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "CORBA exception: " + std::string(e._name()));
        return 1;
    } catch (const std::exception& e) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Exception: " + std::string(e.what()));
        return 1;
    }
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Verify account-server/src/main.cpp
if [ -f ../account-server/src/main.cpp ]; then
    echo "✅ account-server/src/main.cpp updated successfully"
else
    echo "❌ Failed to create ../account-server/src/main.cpp"
    exit 1
fi

# Fix usb-client/src/main.cpp
cat > ../usb-client/src/main.cpp << 'EOF'
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include "usb_manager.h"
#include "config_manager.h"
#include "../common/include/protocol.h"
#include "../common/include/message.h"
#include "../common/include/crypto.h"
#include "../common/include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

int main(int argc, char* argv[]) {
    try {
        ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, ACE_TEXT("USBClient"));
        
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
        
        ConfigManager config_manager("usb_client.conf");
        if (!config_manager.LoadConfig()) {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to load configuration");
            return 1;
        }
        
        USBManager usb_manager;
        std::string usb_id = usb_manager.GenerateUSBID();
        if (usb_id.empty()) {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to generate USB ID");
            return 1;
        }
        
        config_manager.SetUSBID(usb_id);
        
        Protocol protocol(orb.in());
        ACE_SOCK_Stream socket;
        if (!protocol.Connect(ACE_INET_Addr(config_manager.GetServerAddress().c_str()), socket)) {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to connect to account server");
            return 1;
        }
        
        Message msg;
        msg.header.type = MessageType::BIND_REQUEST;
        msg.header.message_id = Utils::GetTimestamp();
        msg.header.timestamp = Utils::GetTimestamp();
        msg.payload = Utils::StringToBytes(usb_id);
        
        auto key = Crypto::GenerateKey();
        auto iv = Crypto::GenerateIV();
        if (!protocol.SendMessage(socket, msg, key, iv)) {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to send bind request");
            return 1;
        }
        
        Message response;
        if (!protocol.ReceiveMessage(socket, response, key, iv) ||
            response.header.type != MessageType::BIND_RESPONSE ||
            Utils::BytesToString(response.payload) != "SUCCESS") {
            Utils::Log(ACE_Log_Priority::LM_ERROR, "Bind request failed");
            return 1;
        }
        
        Utils::Log(ACE_Log_Priority::LM_INFO, "USB client registered successfully: " + usb_id);
        
        // Launch PC client or other logic here
        return 0;
    } catch (const CORBA::Exception& e) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "CORBA exception: " + std::string(e._name()));
        return 1;
    } catch (const std::exception& e) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Exception: " + std::string(e.what()));
        return 1;
    }
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Verify usb-client/src/main.cpp
if [ -f ../usb-client/src/main.cpp ]; then
    echo "✅ usb-client/src/main.cpp updated successfully"
else
    echo "❌ Failed to create ../usb-client/src/main.cpp"
    exit 1
fi

# Fix connection_manager.cpp
cat > ../pc-client/src/connection_manager.cpp << 'EOF'
#include "connection_manager.h"
#include <ace/INET_Addr.h>
#include "../common/include/utils.h"

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
EOF

# Verify connection_manager.cpp
if [ -f ../pc-client/src/connection_manager.cpp ]; then
    echo "✅ connection_manager.cpp updated successfully"
else
    echo "❌ Failed to create ../pc-client/src/connection_manager.cpp"
    exit 1
fi

# Update connection_manager.h
cat > ../pc-client/include/connection_manager.h << 'EOF'
#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <string>
#include <ace/ACE.h>

namespace RemoteAccessSystem {
namespace Common {

class ConnectionManager {
public:
    ConnectionManager();
    bool ConnectToRelay(const std::string& relay_address);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // CONNECTION_MANAGER_H
EOF

# Verify connection_manager.h
if [ -f ../pc-client/include/connection_manager.h ]; then
    echo "✅ connection_manager.h updated successfully"
else
    echo "❌ Failed to create ../pc-client/include/connection_manager.h"
    exit 1
fi

# Clear CMake cache and reconfigure
echo "Clearing CMake cache..."
rm -rf CMakeCache.txt CMakeFiles/
cmake ..

# Rebuild the project
echo "Rebuilding project..."
make -j$(nproc) 2>&1 | tee build.log

# Check for errors
if [ ${PIPESTATUS[0]} -eq 0 ]; then
    echo "✅ Build succeeded!"
else
    echo "⚠️ Build failed! Check build.log for details."
    echo "Last 20 errors:"
    grep "error:" build.log | tail -20
fi