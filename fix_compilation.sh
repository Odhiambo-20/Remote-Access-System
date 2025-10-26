#!/bin/bash

echo "Fixing compilation errors..."
cd ~/Documents/Desktop/remote-access-system

# 1. Fix namespace consistency - use RemoteAccessSystem::Common everywhere
echo "Step 1: Fixing namespaces..."

# Fix common headers to use consistent namespace
find common/include -name "*.h" -exec sed -i 's/namespace RemoteAccess/namespace RemoteAccessSystem::Common/g' {} \;
find common/src -name "*.cpp" -exec sed -i 's/namespace RemoteAccess/namespace RemoteAccessSystem::Common/g' {} \;
find common/src -name "*.cpp" -exec sed -i 's/using namespace RemoteAccess/using namespace RemoteAccessSystem::Common/g' {} \;

# 2. Remove duplicate DatabaseException from account_server_impl.h
echo "Step 2: Removing duplicate DatabaseException..."
cat > account-server/include/account_server_impl.h << 'EOF'
#pragma once

#include <string>
#include <memory>
#include <CORBA.h>
#include "message.h"
#include "database_manager.h"

namespace RemoteAccessSystem {
namespace Common {

// Forward declarations
class DatabaseManager;

class AccountServerImpl {
public:
    explicit AccountServerImpl(DatabaseManager& db_manager);
    virtual ~AccountServerImpl() = default;

    // Account management
    virtual bool RegisterAccount(const std::string& username, const std::string& password);
    virtual bool Authenticate(const std::string& username, const std::string& password);
    virtual bool DeleteAccount(const std::string& username);

    // Device binding
    virtual bool BindDevice(const std::string& username, const std::string& device_id, 
                          const std::string& device_type);
    virtual bool UnbindDevice(const std::string& username, const std::string& device_id);
    virtual std::vector<std::string> GetBoundDevices(const std::string& username);

private:
    DatabaseManager& db_manager_;
};

// Exception class for account server errors
class AccountServerException : public std::runtime_error {
public:
    explicit AccountServerException(const std::string& message) 
        : std::runtime_error(message) {}
};

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# 3. Add missing includes to source files
echo "Step 3: Adding missing includes..."

# Fix usb_manager.cpp
cat > usb-client/src/usb_manager.cpp << 'EOF'
#include "usb_manager.h"
#include "utils.h"
#include <ace/Log_Msg.h>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace RemoteAccessSystem {
namespace Common {

USBManager::USBManager() : ctx_(nullptr) {
    if (libusb_init(&ctx_) < 0) {
        Utils::Log(LM_ERROR, "Failed to initialize libusb");
    }
}

USBManager::~USBManager() {
    if (ctx_) {
        libusb_exit(ctx_);
    }
}

std::string USBManager::GenerateUSBID() {
    std::string device_path = DetectUSBDevice();
    if (device_path.empty()) {
        Utils::Log(LM_ERROR, "No USB device detected");
        return "";
    }
    
    return HardwareID::GenerateID(device_path);
}

std::string USBManager::DetectUSBDevice() {
    // List of potential USB device mount points
    std::vector<std::string> mount_points = {
        "/dev/sda1", "/dev/sdb1", "/dev/sdc1", "/dev/sdd1",
        "/media/usb", "/media/usb0", "/mnt/usb"
    };
    
    for (const auto& mount : mount_points) {
        std::ifstream file(mount);
        if (file.good()) {
            return mount;
        }
    }
    
    // Try libusb approach
    libusb_device **devices;
    ssize_t count = libusb_get_device_list(ctx_, &devices);
    if (count < 0) {
        Utils::Log(LM_ERROR, "Failed to get USB device list");
        return "";
    }
    
    std::string device_path;
    for (ssize_t i = 0; i < count; i++) {
        libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(devices[i], &desc) == 0) {
            // Found a USB device
            std::stringstream ss;
            ss << std::hex << desc.idVendor << ":" << desc.idProduct;
            device_path = ss.str();
            break;
        }
    }
    
    libusb_free_device_list(devices, 1);
    return device_path;
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# Fix config_manager.cpp
cat > usb-client/src/config_manager.cpp << 'EOF'
#include "config_manager.h"
#include "utils.h"
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

ConfigManager::ConfigManager(const std::string& config_file)
    : config_file_(config_file) {}

bool ConfigManager::LoadConfig() {
    if (!Utils::ReadConfig(config_file_, config)) {
        Utils::Log(LM_ERROR, "Failed to load config file: " + config_file_);
        return false;
    }
    
    // Extract required fields
    auto it = config.find("usb_id");
    if (it != config.end()) {
        usb_id_ = it->second;
    }
    
    it = config.find("server_address");
    if (it != config.end()) {
        server_address_ = it->second;
    }
    
    return true;
}

bool ConfigManager::SaveConfig() {
    config["usb_id"] = usb_id_;
    config["server_address"] = server_address_;
    
    if (!Utils::WriteConfig(config_file_, config)) {
        Utils::Log(LM_ERROR, "Failed to save config file: " + config_file_);
        return false;
    }
    
    return true;
}

} // namespace Common
} // namespace RemoteAccessSystem
EOF

# 4. Fix account_manager.cpp to include proper headers
sed -i '1i #include <ace/SOCK_Stream.h>\n#include <ace/SOCK_Connector.h>\n#include <ace/INET_Addr.h>\n#include <ace/Log_Msg.h>\n#include "protocol.h"\n#include "message.h"\n#include "crypto.h"\n#include "utils.h"' pc-client/src/account_manager.cpp

# 5. Fix relay server files
for file in relay-server/src/*.cpp; do
    sed -i '1i #include <ace/SOCK_Stream.h>\n#include <ace/SOCK_Acceptor.h>\n#include <ace/INET_Addr.h>\n#include <ace/Log_Msg.h>\n#include "protocol.h"\n#include "message.h"\n#include "crypto.h"\n#include "utils.h"' "$file"
done

# 6. Fix usb-client main.cpp
sed -i '1i #include <ace/SOCK_Stream.h>\n#include <ace/SOCK_Connector.h>\n#include <ace/INET_Addr.h>\n#include <ace/Log_Msg.h>' usb-client/src/main.cpp

# 7. Fix account-server main.cpp to include proper headers  
sed -i '1i #include <fstream>' account-server/src/main.cpp
sed -i 's/RemoteAccessSystem::Common::DatabaseManager/DatabaseManager/g' account-server/src/main.cpp
sed -i 's/RemoteAccessSystem::Common::ServerManager/ServerManager/g' account-server/src/main.cpp

echo "✅ Fixes applied!"
