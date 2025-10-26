#include "usb_manager.h"
#include "../../common/include/utils.h"
#include <random>
#include <sstream>

namespace RemoteAccessSystem {
namespace Common {

USBManager::USBManager() {}

std::string USBManager::GenerateUSBID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999999);
    std::stringstream ss;
    ss << "USB_" << dis(gen);
    return ss.str();  // Fixed: Added parentheses to call the str() function
}

} // namespace Common
} // namespace RemoteAccessSystem