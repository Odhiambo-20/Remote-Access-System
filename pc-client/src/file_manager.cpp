#include "file_manager.h"
#include "../../common/include/utils.h"
#include <ace/Log_Msg.h>
#include <filesystem>
#include <fstream>

namespace RemoteAccessSystem {
namespace Common {

std::vector<std::string> FileManager::ListDirectory(const std::string& path) {
    std::vector<std::string> files;
    
    if (!IsValidPath(path)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Invalid directory path: %s\n"), path.c_str()));
        return files;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }
    } catch (const std::exception& e) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Error listing directory: %s\n"), e.what()));
    }
    
    return files;
}

std::vector<unsigned char> FileManager::ReadFile(const std::string& path) {
    if (!IsValidPath(path)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Invalid file path: %s\n"), path.c_str()));
        return {};
    }
    
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Cannot open file: %s\n"), path.c_str()));
        return {};
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<unsigned char> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    return data;
}

bool FileManager::WriteFile(const std::string& path, const std::vector<unsigned char>& data) {
    if (!IsValidPath(path)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Invalid file path: %s\n"), path.c_str()));
        return false;
    }
    
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("Cannot create file: %s\n"), path.c_str()));
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

bool FileManager::IsValidPath(const std::string& path) const {
    if (path.empty()) return false;
    
    // Basic validation - check for dangerous patterns
    if (path.find("..") != std::string::npos) return false;
    
    return true;
}

std::string FileManager::GenerateShareLink(const std::string& path) {
    // Generate a random token for the share link
    std::string token = Utils::GenerateRandomToken(32);
    return token;
}

} // namespace Common
} // namespace RemoteAccessSystem