#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

namespace RemoteAccessSystem {
namespace Common {

class Utils {
public:
    static std::string generateDeviceID();
    static std::string hashDeviceID(const std::string& deviceId);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static uint64_t GetTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
};

} // namespace Common
} // namespace RemoteAccessSystem
