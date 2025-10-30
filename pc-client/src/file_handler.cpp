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
    
    // Send registration
    std::string registration = "PC_FILE|" + pcId + "\n";
    send(relaySocket, registration.c_str(), registration.length(), 0);
    
    // Start handler thread
    running = true;
    handlerThread = std::thread(&FileHandler::run, this);
    
    // Start heartbeat thread
    heartbeatThread = std::thread([this]() {
        while (running && relaySocket >= 0) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            if (relaySocket >= 0) {
                std::string heartbeat = "HEARTBEAT\n";
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
    }
    else if (command == "LIST_DIR") {
        std::string id, path;
        std::getline(iss, id, '|');
        std::getline(iss, path);
        handleListDir(path);
    }
    else if (command == "GENERATE_URL") {
        std::string id, filePath;
        std::getline(iss, id, '|');
        std::getline(iss, filePath);
        handleGenerateUrl(filePath);
    }
    else if (command == "DOWNLOAD") {
        std::string id, filePath;
        std::getline(iss, id, '|');
        std::getline(iss, filePath);
        handleDownload(filePath);
    }
}

void FileHandler::handleListDir(const std::string& path)
{
    std::cout << "[FileHandler] Listing directory: " << path << std::endl;
    
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        sendResponse("ERROR|Directory not found\n");
        return;
    }

    std::string response = "DIR_LIST|";
    struct dirent* entry;
    
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string fullPath = path + "/" + name;
        struct stat st;
        
        if (stat(fullPath.c_str(), &st) == 0) {
            std::string type = S_ISDIR(st.st_mode) ? "dir" : "file";
            long size = S_ISREG(st.st_mode) ? st.st_size : 0;
            
            response += name + "|" + type + "|" + std::to_string(size) + ";";
        }
    }
    
    closedir(dir);
    response += "\n";
    
    sendResponse(response);
    std::cout << "[FileHandler] Sent directory listing" << std::endl;
}

void FileHandler::handleGenerateUrl(const std::string& filePath)
{
    std::cout << "[FileHandler] Generating share URL for: " << filePath << std::endl;
    
    // Check if file exists
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        sendResponse("ERROR|File not found\n");
        return;
    }

    // Generate random token
    std::string token = generateToken(32);
    
    // CRITICAL FIX: Add token to HTTPServer instead of local map
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
    std::string shareUrl = "http://" + localIP + ":8080/share/" + token;
    
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
        return;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string response = "DOWNLOAD_START|" + std::to_string(fileSize) + "\n";
    sendResponse(response);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        send(relaySocket, buffer, file.gcount(), 0);
    }

    file.close();
    std::cout << "[FileHandler] File download complete" << std::endl;
}

void FileHandler::sendResponse(const std::string& response)
{
    if (relaySocket >= 0) {
        send(relaySocket, response.c_str(), response.length(), 0);
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