#ifndef USB_MANAGER_H
#define USB_MANAGER_H

#include <string>

namespace RemoteAccessSystem {
namespace Common {

class USBManager {
public:
    USBManager();
    std::string GenerateUSBID();
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // USB_MANAGER_H
