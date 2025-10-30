#include "file_handler.h"
#include "http_server.h"
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

FileHandler::FileHandler(const std::string& pcId, RemoteAccessSystem::Common::HTTPServer* httpServer)
    : pcId(pcId), relaySocket(-1), running(false), httpServer_(httpServer)
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
    
    // FIXED: Send correct registration command that matches relay_server.cpp
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

void FileHandler::run()
{
    char buffer[4096];
    std::string accumulated;

    while (running && relaySocket >= 0) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(relaySocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead <= 0) {
            std::cout << "[FileHandler] Connection closed" << std::endl;
            break;
        }

        accumulated += std::string(buffer, bytesRead);
        
        size_t pos;
        while ((pos = accumulated.find('\n')) != std::string::npos) {
            std::string request = accumulated.substr(0, pos);
            accumulated = accumulated.substr(pos + 1);
            
            std::cout << "[FileHandler] Received request: " << request << std::endl;
            processRequest(request);
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
    else {
        std::cout << "[FileHandler] Unknown command: " << command << std::endl;
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
    std::cout << "[FileHandler] Sent directory listing with " << count << " entries" << std::endl;
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
    
    // Add token to HTTPServer
    if (httpServer_) {
        RemoteAccessSystem::Common::AddShareToken(token, filePath);
        std::cout << "[FileHandler] Token added to HTTPServer: " << token << " -> " << filePath << std::endl;
    } else {
        std::cerr << "[FileHandler] ERROR: HTTPServer is null!" << std::endl;
        sendResponse("ERROR|HTTP server not available\n");
        return;
    }
    
    // Also keep local copy for reference
    shareTokens[token] = filePath;

    // Get local IP address instead of localhost
    std::string localIP = getLocalIPAddress();
    std::string shareUrl = "http://" + localIP + ":8082/share/" + token;
    
    std::string response = "SHARE_URL|" + shareUrl + "\n";
    sendResponse(response);
    
    std::cout << "[FileHandler] Generated share URL: " << shareUrl << std::endl;
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
    std::cout << "[FileHandler] File download complete, sent " << totalSent << " bytes" << std::endl;
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
    std::cout << "[FileHandler] Upload complete: " << received << " bytes received" << std::endl;
    sendResponse("UPLOAD_COMPLETE\n");
}

void FileHandler::sendResponse(const std::string& response)
{
    if (relaySocket >= 0) {
        ssize_t sent = send(relaySocket, response.c_str(), response.length(), 0);
        if (sent < 0) {
            std::cerr << "[FileHandler] Failed to send response" << std::endl;
        }
    } else {
        std::cerr << "[FileHandler] Cannot send response: socket is closed" << std::endl;
    }
}

std::string FileHandler::generateToken(size_t length)
{
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.length() - 1);

    std::string token;
    for (size_t i = 0; i < length; ++i) {
        token += chars[dis(gen)];
    }
    return token;
}