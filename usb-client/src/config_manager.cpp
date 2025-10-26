#include "config_manager.h"
#include "../include/utils.h"

namespace RemoteAccessSystem {
namespace Common {

ConfigManager::ConfigManager(const std::string& config_file) : config_file_(config_file) {}

bool ConfigManager::LoadConfig() {
    std::string config;
    if (!Utils::ReadConfig(config_file_, config)) {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to load config: " + config_file_);
        return false;
    }
    server_address_ = "127.0.0.1:2809";
    return true;
}

std::string ConfigManager::GetServerAddress() const {
    return server_address_;
}

void ConfigManager::SetUSBID(const std::string& usb_id) {
    usb_id_ = usb_id;
}

} // namespace Common
} // namespace RemoteAccessSystem
