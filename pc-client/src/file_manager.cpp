#include <tao/ORB.h>
#include "file_manager.h"
#include "utils.h"
#include <filesystem>
#include <fstream>

namespace RemoteAccessSystem {
namespace Common {

std::vector<std::string> FileManager::ListDirectory(const std::string& path) {
    std::vector<std::string> files;
    if (!IsValidPath(path)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT( "Invalid directory path: " + path);
        return files;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        files.push_back(entry.path().string();
    }
    return files;
}

std::vector<unsigned char> FileManager::ReadFile(const std::string& path) {
    if (!IsValidPath(path)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT( "Invalid file path: " + path);
        return {};
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        ACE_ERROR((LM_ERROR, ACE_TEXT( "Failed to open file: " + path);
        return {};
    }

    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
}

bool FileManager::WriteFile(const std::string& path, const std::vector<unsigned char>& data) {
    if (!IsValidPath(path)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT( "Invalid file path: " + path);
        return false;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        ACE_ERROR((LM_ERROR, ACE_TEXT( "Failed to write file: " + path);
        return false;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size();
    return true;
}

std::string FileManager::GenerateShareLink(const std::string& path) {
    if (!IsValidPath(path)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT( "Invalid file path for share link: " + path);
        return "";
    }

    std::string token = RemoteAccessSystem::Common::Crypto::GenerateRandomString(32);
    std::string link = "http://localhost:8080/share/" + token;
    // In production, store token-path mapping in a database
    ACE_DEBUG((LM_INFO, ACE_TEXT( "Generated share link: " + link);
    return link;
}

bool FileManager::IsValidPath(const std::string& path) const {
    return !path.empty() && std::filesystem::exists(path);
}

} // namespace Common
} // namespace RemoteAccessSystem