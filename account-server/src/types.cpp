#include "../include/types.h"
#include "../include/utils.h"
#include <ace/Log_Msg.h>

namespace RemoteAccessSystem {
namespace Common {

AccountServerConfig AccountServerConfig::loadFromFile(const std::string& config_file) {
    AccountServerConfig config;
    std::string content;
    if (Utils::ReadConfig(config_file, content)) {
        config.database_path = "account.db";
        config.session_timeout = 3600;
        config.max_bindings_per_device = 5;
        config.heartbeat_timeout = 300;
    } else {
        Utils::Log(ACE_Log_Priority::LM_ERROR, "Failed to load config: " + config_file);
    }
    return config;
}

AccountServerConfig AccountServerConfig::defaultConfig() {
    AccountServerConfig config;
    config.database_path = "account.db";
    config.session_timeout = 3600;
    config.max_bindings_per_device = 5;
    config.heartbeat_timeout = 300;
    return config;
}

} // namespace Common
} // namespace RemoteAccessSystem
