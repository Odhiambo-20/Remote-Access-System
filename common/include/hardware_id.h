#ifndef HARDWARE_ID_H
#define HARDWARE_ID_H

#include <string>
#include <vector>

namespace RemoteAccessSystem {
namespace Common {

class HardwareID {
public:
    // Generate a unique ID for a USB device based on its hardware properties
    static std::string GenerateUSBID(const std::string& device_path);

    // Generate a unique ID for a PC based on its hardware components
    static std::string GeneratePCID();

    // Validate a hardware ID format (e.g., length, characters)
    static bool ValidateID(const std::string& id);

private:
    // Combine hardware attributes into a unique ID
    static std::string CombineAttributes(const std::vector<std::string>& attributes);

    // Platform-specific functions to retrieve hardware details
    #ifdef _WIN32
    static std::string GetWindowsCPUInfo();
    static std::string GetWindowsMotherboardInfo();
    static std::string GetWindowsDiskInfo();
    static std::string GetWindowsUSBInfo(const std::string& device_path);
    #elif defined(__linux__)
    static std::string GetLinuxCPUInfo();
    static std::string GetLinuxMotherboardInfo();
    static std::string GetLinuxDiskInfo();
    static std::string GetLinuxUSBInfo(const std::string& device_path);
    #elif defined(__APPLE__)
    static std::string GetMacCPUInfo();
    static std::string GetMacMotherboardInfo();
    static std::string GetMacDiskInfo();
    static std::string GetMacUSBInfo(const std::string& device_path);
    #endif
};

} // namespace Common
} // namespace RemoteAccessSystem::Common

#endif // HARDWARE_ID_HS
