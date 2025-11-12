#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <map>
#include <thread>
#include <atomic>

// Forward declarations
namespace RemoteAccessSystem {
namespace Common {
    class HTTPServer;
}
}

class FileServer;

class FileHandler {
public:
    FileHandler(const std::string& pcId, 
                RemoteAccessSystem::Common::HTTPServer* httpServer,
                FileServer* fileServer);
    ~FileHandler();

    int connect_to_relay(const std::string& host, int port);
    void shutdown();
    std::string getFilePath(const std::string& token);

private:
    std::string pcId;
    std::string relayHost;
    int relayPort;
    int relaySocket;
    std::atomic<bool> running;
    std::thread handlerThread;
    std::thread heartbeatThread;
    std::map<std::string, std::string> shareTokens;
    RemoteAccessSystem::Common::HTTPServer* httpServer_;
    FileServer* fileServer_;

    void run();
    void processRequest(const std::string& request);
    
    // Command handlers
    void handleListDir(const std::string& path);
    void handleGenerateUrl(const std::string& filePath);
    void handleDownload(const std::string& filePath);
    void handleUpload(const std::string& remotePath, long long fileSize);
    void handleDelete(const std::string& filePath);
    void handleRename(const std::string& oldPath, const std::string& newPath);
    void handleCopy(const std::string& srcPath, const std::string& destPath);
    void handleCreateFolder(const std::string& folderPath);
    
    // Helper functions
    bool copyFile(const std::string& src, const std::string& dest);
    bool copyDirectory(const std::string& src, const std::string& dest);
    bool removeDirectory(const std::string& path);
    
    void sendResponse(const std::string& response);
    std::string generateToken(size_t length);
    std::string getLocalIPAddress();
};

#endif // FILE_HANDLER_H