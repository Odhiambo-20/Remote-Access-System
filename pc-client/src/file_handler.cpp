#include "file_handler.h"
#include "http_server.h"
#include "file_server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <random>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/types.h>

FileHandler::FileHandler(const std::string& pcId, 
                         RemoteAccessSystem::Common::HTTPServer* httpServer,
                         FileServer* fileServer)
    : pcId(pcId), relaySocket(-1), running(false), 
      httpServer_(httpServer), fileServer_(fileServer)
{
}

FileHandler::~FileHandler()
{
    shutdown();
}

std::string FileHandler::getFilePath(const std::string& token)
{
    auto it = shareTokens.find(token);
    if (it != shareTokens.end()) {
        return it->second;
    }
    return "";
}

std::string FileHandler::getLocalIPAddress()
{
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];
    std::string result = "localhost";
    
    if (getifaddrs(&ifaddr) == -1) {
        std::cerr << "[FileHandler] getifaddrs failed" << std::endl;
        return result;
    }
    
    // Look for IPv4 address that's not localhost
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr)
            continue;
        
        int family = ifa->ifa_addr->sa_family;
        
        // Check for IPv4
        if (family == AF_INET) {
            int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                              host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
            
            if (s != 0) {
                continue;
            }
            
            std::string ip(host);
            
            // Skip loopback
            if (ip == "127.0.0.1") {
                continue;
            }
            
            // Prefer common local network ranges
            if (ip.substr(0, 8) == "192.168." || 
                ip.substr(0, 3) == "10." || 
                ip.substr(0, 4) == "172.") {
                result = ip;
                std::cout << "[FileHandler] Using local IP: " << ip << std::endl;
                freeifaddrs(ifaddr);
                return result;
            }
            
            // Store any non-loopback IPv4 as fallback
            if (result == "localhost") {
                result = ip;
            }
        }
    }
    
    freeifaddrs(ifaddr);
    
    if (result != "localhost") {
        std::cout << "[FileHandler] Using fallback IP: " << result << std::endl;
    } else {
        std::cout << "[FileHandler] WARNING: Could not find local IP, using localhost" << std::endl;
    }
    
    return result;
}

int FileHandler::connect_to_relay(const std::string& host, int port)
{
    relayHost = host;
    relayPort = port;
    
    relaySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (relaySocket < 0) {
        std::cerr << "[FileHandler] Failed to create socket" << std::endl;
        return -1;
    }

    // Set socket options to prevent premature closure
    int keepalive = 1;
    setsockopt(relaySocket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    
    struct timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    setsockopt(relaySocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(relaySocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "[FileHandler] Invalid address" << std::endl;
        close(relaySocket);
        relaySocket = -1;
        return -1;
    }

    if (connect(relaySocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "[FileHandler] Connection failed" << std::endl;
        close(relaySocket);
        relaySocket = -1;
        return -1;
    }

    std::cout << "[FileHandler] Connected to relay server" << std::endl;
    
    // Send correct registration command that matches relay_server.cpp
    std::string registration = "FILE_HANDLER_REGISTER|" + pcId + "\n";
    send(relaySocket, registration.c_str(), registration.length(), 0);
    std::cout << "[FileHandler] Sent registration: " << registration << std::endl;
    
    // Wait for registration confirmation
    char response[256];
    memset(response, 0, sizeof(response));
    int bytesRead = recv(relaySocket, response, sizeof(response) - 1, 0);
    if (bytesRead > 0) {
        std::string resp(response, bytesRead);
        std::cout << "[FileHandler] Registration response: " << resp << std::endl;
        
        if (resp.find("OK|FILE_HANDLER_REGISTERED") != std::string::npos) {
            std::cout << "[FileHandler] ✅ File handler registered successfully!" << std::endl;
        } else if (resp.find("ERROR") != std::string::npos) {
            std::cerr << "[FileHandler] ❌ Registration failed: " << resp << std::endl;
            close(relaySocket);
            relaySocket = -1;
            return -1;
        }
    } else {
        std::cerr << "[FileHandler] No response from relay server" << std::endl;
    }
    
    // Start handler thread
    running = true;
    handlerThread = std::thread(&FileHandler::run, this);
    
    // Start heartbeat thread
    heartbeatThread = std::thread([this]() {
        while (running && relaySocket >= 0) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            if (relaySocket >= 0) {
                std::string heartbeat = "HEARTBEAT|" + pcId + "\n";
                send(relaySocket, heartbeat.c_str(), heartbeat.length(), 0);
                std::cout << "[FileHandler] Heartbeat sent" << std::endl;
            }
        }
    });
    
    return 0;
}

void FileHandler::shutdown()
{
    running = false;
    if (relaySocket >= 0) {
        close(relaySocket);
        relaySocket = -1;
    }
    if (handlerThread.joinable()) {
        handlerThread.join();
    }
    if (heartbeatThread.joinable()) {
        heartbeatThread.join();
    }
}

void FileHandler::sendResponse(const std::string& response)
{
    if (relaySocket < 0) {
        std::cerr << "[FileHandler] Cannot send response: socket not connected" << std::endl;
        return;
    }
    
    ssize_t bytesSent = send(relaySocket, response.c_str(), response.length(), 0);
    if (bytesSent < 0) {
        std::cerr << "[FileHandler] Failed to send response: " << strerror(errno) << std::endl;
    } else if (static_cast<size_t>(bytesSent) < response.length()) {
        std::cerr << "[FileHandler] Partial send: " << bytesSent << "/" << response.length() << " bytes" << std::endl;
    } else {
        std::cout << "[FileHandler] Sent response: " << response.substr(0, 50) 
                  << (response.length() > 50 ? "..." : "") << std::endl;
    }
}

std::string FileHandler::generateToken(unsigned long length)
{
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const size_t charsetSize = sizeof(charset) - 1;
    
    std::string token;
    token.reserve(length);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charsetSize - 1);
    
    for (unsigned long i = 0; i < length; ++i) {
        token += charset[dis(gen)];
    }
    
    std::cout << "[FileHandler] Generated token: " << token << std::endl;
    return token;
}

void FileHandler::run()
{
    char buffer[4096];
    std::string accumulated;

    while (running && relaySocket >= 0) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(relaySocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "[FileHandler] Connection closed by relay server" << std::endl;
            } else {
                std::cerr << "[FileHandler] recv error: " << strerror(errno) << std::endl;
            }
            break;
        }

        accumulated += std::string(buffer, bytesRead);
        
        size_t pos;
        while ((pos = accumulated.find('\n')) != std::string::npos) {
            std::string request = accumulated.substr(0, pos);
            accumulated = accumulated.substr(pos + 1);
            
            std::cout << "[FileHandler] Received request: " << request << std::endl;
            
            try {
                processRequest(request);
            } catch (const std::exception& e) {
                std::cerr << "[FileHandler] Exception processing request: " << e.what() << std::endl;
                sendResponse("ERROR|Internal error processing request\n");
            }
        }
    }
}

void FileHandler::processRequest(const std::string& request)
{
    std::istringstream iss(request);
    std::string command;
    std::getline(iss, command, '|');

    if (command == "PONG") {
        std::cout << "[FileHandler] Received PONG from server" << std::endl;
    }
    else if (command == "OK") {
        // Acknowledgment
        std::cout << "[FileHandler] Received OK acknowledgment" << std::endl;
    }
    else if (command == "LIST_DIR") {
        std::string id, path;
        std::getline(iss, id, '|');
        std::getline(iss, path);
        std::cout << "[FileHandler] Processing LIST_DIR for path: " << path << std::endl;
        handleListDir(path);
    }
    else if (command == "GENERATE_URL") {
        std::string id, filePath;
        std::getline(iss, id, '|');
        std::getline(iss, filePath);
        std::cout << "[FileHandler] Processing GENERATE_URL for: " << filePath << std::endl;
        handleGenerateUrl(filePath);
    }
    else if (command == "DOWNLOAD") {
        std::string id, filePath;
        std::getline(iss, id, '|');
        std::getline(iss, filePath);
        std::cout << "[FileHandler] Processing DOWNLOAD for: " << filePath << std::endl;
        handleDownload(filePath);
    }
    else if (command == "UPLOAD") {
        std::string id, remotePath, sizeStr;
        std::getline(iss, id, '|');
        std::getline(iss, remotePath, '|');
        std::getline(iss, sizeStr);
        long long fileSize = std::stoll(sizeStr);
        std::cout << "[FileHandler] Processing UPLOAD to: " << remotePath 
                  << " size: " << fileSize << " bytes" << std::endl;
        handleUpload(remotePath, fileSize);
    }
    else if (command == "DELETE") {
        std::string id, filePath;
        std::getline(iss, id, '|');
        std::getline(iss, filePath);
        std::cout << "[FileHandler] Processing DELETE for: " << filePath << std::endl;
        handleDelete(filePath);
    }
    else if (command == "RENAME") {
        std::string id, oldPath, newPath;
        std::getline(iss, id, '|');
        std::getline(iss, oldPath, '|');
        std::getline(iss, newPath);
        std::cout << "[FileHandler] Processing RENAME from: " << oldPath << " to: " << newPath << std::endl;
        handleRename(oldPath, newPath);
    }
    else if (command == "COPY") {
        std::string id, srcPath, destPath;
        std::getline(iss, id, '|');
        std::getline(iss, srcPath, '|');
        std::getline(iss, destPath);
        std::cout << "[FileHandler] Processing COPY from: " << srcPath << " to: " << destPath << std::endl;
        handleCopy(srcPath, destPath);
    }
    else if (command == "CREATE_FOLDER") {
        std::string id, folderPath;
        std::getline(iss, id, '|');
        std::getline(iss, folderPath);
        std::cout << "[FileHandler] Processing CREATE_FOLDER at: " << folderPath << std::endl;
        handleCreateFolder(folderPath);
    }
    else {
        std::cout << "[FileHandler] ⚠️  Unknown command: " << command << std::endl;
        sendResponse("ERROR|Unknown command: " + command + "\n");
    }
}

void FileHandler::handleListDir(const std::string& path)
{
    std::cout << "[FileHandler] Listing directory: " << path << std::endl;
    
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::string errorMsg = "ERROR|Directory not found: " + path + "\n";
        sendResponse(errorMsg);
        std::cerr << "[FileHandler] " << errorMsg;
        return;
    }

    std::string response = "DIR_LIST|";
    struct dirent* entry;
    int count = 0;
    
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string fullPath = path + "/" + name;
        struct stat st;
        
        if (stat(fullPath.c_str(), &st) == 0) {
            std::string type = S_ISDIR(st.st_mode) ? "dir" : "file";
            long size = S_ISREG(st.st_mode) ? st.st_size : 0;
            
            response += name + "|" + type + "|" + std::to_string(size) + ";";
            count++;
        }
    }
    
    closedir(dir);
    response += "\n";
    
    sendResponse(response);
    std::cout << "[FileHandler] ✅ Sent directory listing with " << count << " entries" << std::endl;
}

void FileHandler::handleGenerateUrl(const std::string& filePath)
{
    std::cout << "[FileHandler] Generating share URL for: " << filePath << std::endl;
    
    // Check if file exists
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        sendResponse("ERROR|File not found\n");
        std::cerr << "[FileHandler] File not found: " << filePath << std::endl;
        return;
    }

    // Generate random token
    std::string token = generateToken(32);
    
    // Add token to FileServer (not HTTPServer!)
    if (fileServer_) {
        fileServer_->addShareToken(QString::fromStdString(token), 
                                   QString::fromStdString(filePath));
        std::cout << "[FileHandler] ✅ Token added to FileServer: " << token 
                  << " -> " << filePath << std::endl;
    } else {
        std::cerr << "[FileHandler] ERROR: FileServer is null!" << std::endl;
        sendResponse("ERROR|File server not available\n");
        return;
    }
    
    // Also keep local copy for reference
    shareTokens[token] = filePath;

    // Get local IP address and FileServer port
    std::string localIP = getLocalIPAddress();
    int fileServerPort = fileServer_ ? fileServer_->getHttpPort() : 8082;
    std::string shareUrl = "http://" + localIP + ":" + 
                          std::to_string(fileServerPort) + "/share/" + token;
    
    std::string response = "SHARE_URL|" + shareUrl + "\n";
    sendResponse(response);
    
    std::cout << "[FileHandler] ✅ Generated share URL: " << shareUrl << std::endl;
}

void FileHandler::handleDownload(const std::string& filePath)
{
    std::cout << "[FileHandler] Downloading file: " << filePath << std::endl;
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        sendResponse("ERROR|File not found\n");
        std::cerr << "[FileHandler] Cannot open file: " << filePath << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string response = "DOWNLOAD_START|" + std::to_string(fileSize) + "\n";
    sendResponse(response);
    std::cout << "[FileHandler] Sending file, size: " << fileSize << " bytes" << std::endl;

    char buffer[8192];
    size_t totalSent = 0;
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        ssize_t sent = send(relaySocket, buffer, file.gcount(), 0);
        if (sent <= 0) {
            std::cerr << "[FileHandler] Send failed during download" << std::endl;
            break;
        }
        totalSent += sent;
    }

    file.close();
    std::cout << "[FileHandler] ✅ File download complete, sent " << totalSent << " bytes" << std::endl;
}

void FileHandler::handleUpload(const std::string& remotePath, long long fileSize)
{
    std::cout << "[FileHandler] Receiving upload to: " << remotePath 
              << " size: " << fileSize << " bytes" << std::endl;
    
    // Send ready signal
    std::string response = "UPLOAD_READY\n";
    sendResponse(response);
    
    // Open file for writing
    std::ofstream outFile(remotePath, std::ios::binary);
    if (!outFile.is_open()) {
        sendResponse("ERROR|Cannot create file\n");
        std::cerr << "[FileHandler] Cannot create file: " << remotePath << std::endl;
        return;
    }
    
    // Receive file data
    char buffer[8192];
    long long received = 0;
    
    while (received < fileSize) {
        size_t toRead = std::min((long long)sizeof(buffer), fileSize - received);
        ssize_t bytesRead = recv(relaySocket, buffer, toRead, 0);
        
        if (bytesRead <= 0) {
            std::cerr << "[FileHandler] Connection lost during upload" << std::endl;
            outFile.close();
            sendResponse("ERROR|Upload interrupted\n");
            return;
        }
        
        outFile.write(buffer, bytesRead);
        received += bytesRead;
        
        if (received % 1048576 == 0) { // Log every 1MB
            std::cout << "[FileHandler] Upload progress: " << received << "/" << fileSize << " bytes" << std::endl;
        }
    }
    
    outFile.close();
    std::cout << "[FileHandler] ✅ Upload complete: " << received << " bytes received" << std::endl;
    sendResponse("UPLOAD_COMPLETE\n");
}

void FileHandler::handleDelete(const std::string& filePath)
{
    std::cout << "[FileHandler] Deleting: " << filePath << std::endl;
    
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        std::string errorMsg = "ERROR|File not found: " + filePath + "\n";
        sendResponse(errorMsg);
        std::cerr << "[FileHandler] File not found: " << filePath << std::endl;
        return;
    }
    
    // Check if it's a directory
    if (S_ISDIR(st.st_mode)) {
        // Delete directory recursively
        if (removeDirectory(filePath)) {
            sendResponse("DELETE_OK\n");
            std::cout << "[FileHandler] ✅ Directory deleted successfully: " << filePath << std::endl;
        } else {
            std::string errorMsg = "ERROR|Failed to delete directory: " + std::string(strerror(errno)) + "\n";
            sendResponse(errorMsg);
            std::cerr << "[FileHandler] Failed to delete directory: " << filePath << std::endl;
        }
    } else {
        // Delete file
        if (remove(filePath.c_str()) == 0) {
            sendResponse("DELETE_OK\n");
            std::cout << "[FileHandler] ✅ File deleted successfully: " << filePath << std::endl;
        } else {
            std::string errorMsg = "ERROR|Failed to delete file: " + std::string(strerror(errno)) + "\n";
            sendResponse(errorMsg);
            std::cerr << "[FileHandler] Failed to delete file: " << filePath << " - " << strerror(errno) << std::endl;
        }
    }
}

void FileHandler::handleRename(const std::string& oldPath, const std::string& newPath)
{
    std::cout << "[FileHandler] Renaming: " << oldPath << " to " << newPath << std::endl;
    
    struct stat st;
    if (stat(oldPath.c_str(), &st) != 0) {
        std::string errorMsg = "ERROR|Source file not found: " + oldPath + "\n";
        sendResponse(errorMsg);
        std::cerr << "[FileHandler] Source file not found: " << oldPath << std::endl;
        return;
    }
    
    if (rename(oldPath.c_str(), newPath.c_str()) == 0) {
        sendResponse("RENAME_OK\n");
        std::cout << "[FileHandler] ✅ Renamed successfully: " << oldPath << " -> " << newPath << std::endl;
    } else {
        std::string errorMsg = "ERROR|Failed to rename: " + std::string(strerror(errno)) + "\n";
        sendResponse(errorMsg);
        std::cerr << "[FileHandler] Failed to rename: " << strerror(errno) << std::endl;
    }
}

void FileHandler::handleCopy(const std::string& srcPath, const std::string& destPath)
{
    std::cout << "[FileHandler] Copying: " << srcPath << " to " << destPath << std::endl;
    
    struct stat st;
    if (stat(srcPath.c_str(), &st) != 0) {
        std::string errorMsg = "ERROR|Source file not found: " + srcPath + "\n";
        sendResponse(errorMsg);
        std::cerr << "[FileHandler] Source file not found: " << srcPath << std::endl;
        return;
    }
    
    // Determine destination path
    std::string finalDestPath = destPath;
    struct stat destSt;
    if (stat(destPath.c_str(), &destSt) == 0 && S_ISDIR(destSt.st_mode)) {
        // destPath is a directory, append source filename
        size_t lastSlash = srcPath.find_last_of('/');
        std::string filename = (lastSlash != std::string::npos) ? srcPath.substr(lastSlash + 1) : srcPath;
        finalDestPath = destPath + "/" + filename;
    }
    
    if (S_ISDIR(st.st_mode)) {
        // Copy directory recursively
        if (copyDirectory(srcPath, finalDestPath)) {
            sendResponse("COPY_OK\n");
            std::cout << "[FileHandler] ✅ Directory copied successfully" << std::endl;
        } else {
            std::string errorMsg = "ERROR|Failed to copy directory: " + std::string(strerror(errno)) + "\n";
            sendResponse(errorMsg);
            std::cerr << "[FileHandler] Failed to copy directory" << std::endl;
        }
    } else {
        // Copy file
        if (copyFile(srcPath, finalDestPath)) {
            sendResponse("COPY_OK\n");
            std::cout << "[FileHandler] ✅ File copied successfully" << std::endl;
        } else {
            std::string errorMsg = "ERROR|Failed to copy file: " + std::string(strerror(errno)) + "\n";
            sendResponse(errorMsg);
            std::cerr << "[FileHandler] Failed to copy file" << std::endl;
        }
    }
}

void FileHandler::handleCreateFolder(const std::string& folderPath)
{
    std::cout << "[FileHandler] Creating folder: " << folderPath << std::endl;
    
    // Check if path already exists
    struct stat st;
    if (stat(folderPath.c_str(), &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            std::string errorMsg = "ERROR|Folder already exists: " + folderPath + "\n";
            sendResponse(errorMsg);
            std::cerr << "[FileHandler] Folder already exists: " << folderPath << std::endl;
        } else {
            std::string errorMsg = "ERROR|Path exists but is not a directory: " + folderPath + "\n";
            sendResponse(errorMsg);
            std::cerr << "[FileHandler] Path exists but is not a directory: " << folderPath << std::endl;
        }
        return;
    }
    
    // Create the directory with permissions 0755 (rwxr-xr-x)
    if (mkdir(folderPath.c_str(), 0755) == 0) {
        sendResponse("CREATE_FOLDER_OK\n");
        std::cout << "[FileHandler] ✅ Folder created successfully: " << folderPath << std::endl;
    } else {
        std::string errorMsg = "ERROR|Failed to create folder: " + std::string(strerror(errno)) + "\n";
        sendResponse(errorMsg);
        std::cerr << "[FileHandler] Failed to create folder: " << folderPath 
                  << " - " << strerror(errno) << std::endl;
    }
}

bool FileHandler::copyFile(const std::string& src, const std::string& dest)
{
    std::ifstream srcFile(src, std::ios::binary);
    if (!srcFile.is_open()) {
        std::cerr << "[FileHandler] Cannot open source file: " << src << std::endl;
        return false;
    }
    
    std::ofstream destFile(dest, std::ios::binary);
    if (!destFile.is_open()) {
        std::cerr << "[FileHandler] Cannot create destination file: " << dest << std::endl;
        return false;
    }
    
    char buffer[8192];
    while (srcFile.read(buffer, sizeof(buffer)) || srcFile.gcount() > 0) {
        destFile.write(buffer, srcFile.gcount());
        if (!destFile) {
            std::cerr << "[FileHandler] Write error during copy" << std::endl;
            srcFile.close();
            destFile.close();
            return false;
        }
    }
    
    srcFile.close();
    destFile.close();
    
    // Copy permissions
    struct stat st;
    if (stat(src.c_str(), &st) == 0) {
        chmod(dest.c_str(), st.st_mode);
    }
    
    return true;
}

bool FileHandler::copyDirectory(const std::string& src, const std::string& dest)
{
    // Create destination directory
    if (mkdir(dest.c_str(), 0755) != 0 && errno != EEXIST) {
        std::cerr << "[FileHandler] Cannot create directory: " << dest 
                  << " - " << strerror(errno) << std::endl;
        return false;
    }
    
    DIR* dir = opendir(src.c_str());
    if (!dir) {
        std::cerr << "[FileHandler] Cannot open source directory: " << src << std::endl;
        return false;
    }
    
    struct dirent* entry;
    bool success = true;
    
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        
        std::string srcPath = src + "/" + name;
        std::string destPath = dest + "/" + name;
        
        struct stat st;
        if (stat(srcPath.c_str(), &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                if (!copyDirectory(srcPath, destPath)) {
                    success = false;
                    std::cerr << "[FileHandler] Failed to copy subdirectory: " << srcPath << std::endl;
                }
            } else {
                if (!copyFile(srcPath, destPath)) {
                    success = false;
                    std::cerr << "[FileHandler] Failed to copy file: " << srcPath << std::endl;
                }
            }
        }
    }
    
    closedir(dir);
    return success;
}

bool FileHandler::removeDirectory(const std::string& path)
{
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "[FileHandler] Cannot open directory for deletion: " << path << std::endl;
        return false;
    }
    
    struct dirent* entry;
    bool success = true;
    
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        
        std::string fullPath = path + "/" + name;
        struct stat st;
        
        if (stat(fullPath.c_str(), &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                if (!removeDirectory(fullPath)) {
                    success = false;
                }
            } else {
                if (remove(fullPath.c_str()) != 0) {
                    std::cerr << "[FileHandler] Failed to delete file: " << fullPath 
                              << " - " << strerror(errno) << std::endl;
                    success = false;
                }
            }
        }
    }
    
    closedir(dir);
    
    if (success) {
        if (rmdir(path.c_str()) == 0) {
            return true;
        } else {
            std::cerr << "[FileHandler] Failed to remove directory: " << path 
                      << " - " << strerror(errno) << std::endl;
            return false;
        }
    }
    
    return false;
}