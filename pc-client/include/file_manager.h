#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

namespace RemoteAccessSystem {
namespace Common {

class FileManager {
public:
    FileManager() = default;
    ~FileManager() = default;

    // List files in a directory
    std::vector<std::string> ListDirectory(const std::string& path);

    // Read a file's contents
    std::vector<unsigned char> ReadFile(const std::string& path);

    // Write data to a file
    bool WriteFile(const std::string& path, const std::vector<unsigned char>& data);

    // Generate a shareable link for a file
    std::string GenerateShareLink(const std::string& path);

private:
    // Validate file path
    bool IsValidPath(const std::string& path) const;
};

} // namespace Common
} // namespace RemoteAccessSystem

#endif // FILE_MANAGER_H