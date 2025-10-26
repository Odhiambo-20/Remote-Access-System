#include "../include/hardware_id.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <openssl/sha.h>

#ifdef __linux__
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#endif

namespace RemoteAccessSystem {
namespace Common {

std::string HardwareID::GenerateUSBID(const std::string& device_path) {
#ifdef __linux__
    std::string usb_info = GetLinuxUSBInfo(device_path);
    std::vector<std::string> attrs = {usb_info, device_path};
    return CombineAttributes(attrs);
#else
    return "USB-UNSUPPORTED-PLATFORM";
#endif
}

std::string HardwareID::GeneratePCID() {
#ifdef __linux__
    std::vector<std::string> attributes;
    attributes.push_back(GetLinuxCPUInfo());
    attributes.push_back(GetLinuxMotherboardInfo());
    attributes.push_back(GetLinuxDiskInfo());
    return CombineAttributes(attributes);
#elif defined(_WIN32)
    std::vector<std::string> attributes;
    attributes.push_back(GetWindowsCPUInfo());
    attributes.push_back(GetWindowsMotherboardInfo());
    attributes.push_back(GetWindowsDiskInfo());
    return CombineAttributes(attributes);
#elif defined(__APPLE__)
    std::vector<std::string> attributes;
    attributes.push_back(GetMacCPUInfo());
    attributes.push_back(GetMacMotherboardInfo());
    attributes.push_back(GetMacDiskInfo());
    return CombineAttributes(attributes);
#else
    return "PC-UNSUPPORTED-PLATFORM";
#endif
}

bool HardwareID::ValidateID(const std::string& id) {
    // Basic validation: check minimum length and format
    if (id.length() < 10) {
        return false;
    }
    
    // Check for valid prefix
    if (id.substr(0, 3) != "PC-" && id.substr(0, 4) != "USB-") {
        return false;
    }
    
    return true;
}

std::string HardwareID::CombineAttributes(const std::vector<std::string>& attributes) {
    std::stringstream combined;
    for (const auto& attr : attributes) {
        combined << attr;
    }
    
    // Hash the combined string using SHA256
    std::string data = combined.str();
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);
    
    // Convert hash to hex string
    std::stringstream ss;
    ss << "PC-";
    for (int i = 0; i < 16; i++) { // Use first 16 bytes for shorter ID
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

#ifdef __linux__
std::string HardwareID::GetLinuxCPUInfo() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    std::string cpu_id;
    
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            cpu_id = line;
            break;
        }
    }
    
    return cpu_id.empty() ? "unknown-cpu" : cpu_id;
}

std::string HardwareID::GetLinuxMotherboardInfo() {
    std::ifstream product_uuid("/sys/class/dmi/id/product_uuid");
    std::string uuid;
    
    if (product_uuid.is_open()) {
        std::getline(product_uuid, uuid);
        return uuid;
    }
    
    return "unknown-motherboard";
}

std::string HardwareID::GetLinuxDiskInfo() {
    std::ifstream disk_id("/sys/class/block/sda/device/wwid");
    std::string id;
    
    if (disk_id.is_open()) {
        std::getline(disk_id, id);
        return id;
    }
    
    // Fallback: try to get any disk serial
    std::ifstream partitions("/proc/partitions");
    if (partitions.is_open()) {
        return "disk-from-partitions";
    }
    
    return "unknown-disk";
}

std::string HardwareID::GetLinuxUSBInfo(const std::string& device_path) {
    // Try to read USB device info from sysfs
    std::string sysfs_path = device_path;
    if (sysfs_path.empty()) {
        return "unknown-usb";
    }
    
    std::ifstream vendor(sysfs_path + "/idVendor");
    std::ifstream product(sysfs_path + "/idProduct");
    std::ifstream serial(sysfs_path + "/serial");
    
    std::string vid, pid, sn;
    
    if (vendor.is_open()) std::getline(vendor, vid);
    if (product.is_open()) std::getline(product, pid);
    if (serial.is_open()) std::getline(serial, sn);
    
    return vid + "-" + pid + "-" + sn;
}
#endif

#ifdef _WIN32
std::string HardwareID::GetWindowsCPUInfo() {
    return "windows-cpu-info";
}

std::string HardwareID::GetWindowsMotherboardInfo() {
    return "windows-motherboard-info";
}

std::string HardwareID::GetWindowsDiskInfo() {
    return "windows-disk-info";
}

std::string HardwareID::GetWindowsUSBInfo(const std::string& device_path) {
    return "windows-usb-info";
}
#endif

#ifdef __APPLE__
std::string HardwareID::GetMacCPUInfo() {
    return "mac-cpu-info";
}

std::string HardwareID::GetMacMotherboardInfo() {
    return "mac-motherboard-info";
}

std::string HardwareID::GetMacDiskInfo() {
    return "mac-disk-info";
}

std::string HardwareID::GetMacUSBInfo(const std::string& device_path) {
    return "mac-usb-info";
}
#endif

} // namespace Common
} // namespace RemoteAccessSystem
