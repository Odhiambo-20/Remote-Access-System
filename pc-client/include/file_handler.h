#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <map>
#include <thread>
#include <atomic>

// Forward declaration
namespace RemoteAccessSystem {
namespace Common {
    class HTTPServer;
}
}

class FileHandler {
public:
    FileHandler(const std::string& pcId, RemoteAccessSystem::Common::HTTPServer* httpServer);
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

    void run();
    void processRequest(const std::string& request);
    void handleListDir(const std::string& path);
    void handleGenerateUrl(const std::string& filePath);
    void handleDownload(const std::string& filePath);
    void handleUpload(const std::string& remotePath, long long fileSize);  // ADD THIS LINE
    void sendResponse(const std::string& response);
    std::string generateToken(size_t length);
    std::string getLocalIPAddress();
};

#endif // FILE_HANDLER_H