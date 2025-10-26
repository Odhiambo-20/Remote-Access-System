#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>

namespace RemoteAccessSystem {
namespace Common {

class ConfigManager {
public:
    explicit ConfigManager(const std::string& config_file);
    bool LoadConfig();
    std::string GetServerAddress() const;
    void SetUSBID(const std::string& usb_id);

private:
    std::string config_file_;
    std::string server_address_;
    std::string usb_id_;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // CONFIG_MANAGER_H
