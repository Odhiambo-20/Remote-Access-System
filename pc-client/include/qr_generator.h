#ifndef QR_GENERATOR_H
#define QR_GENERATOR_H

#include <string>
#include <vector>

namespace RemoteAccessSystem {
namespace Common {

class QRGenerator {
public:
    static std::vector<uint8_t> GeneratePNG(const std::string& data, int size = 300);
    static std::string GenerateSVG(const std::string& data, int size = 300);
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // QR_GENERATOR_H
