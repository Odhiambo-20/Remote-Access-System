#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

class DesktopAgent {
public:
    DesktopAgent(const std::string& relay_host, int relay_port)
        : relay_host_(relay_host), relay_port_(relay_port), running_(false) {
        
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        hostname_ = std::string(hostname);
        
        const char* username = getenv("USER");
        username_ = username ? std::string(username) : "unknown";
    }
    
    int connect_to_relay() {
        ACE_INET_Addr addr(relay_port_, relay_host_.c_str());
        ACE_SOCK_Connector connector;
        
        if (connector.connect(stream_, addr) == -1) {
            std::cerr << "[Agent] Failed to connect to relay server" << std::endl;
            return -1;
        }
        
        std::cout << "[Agent] Connected to relay server at " << relay_host_ << ":" << relay_port_ << std::endl;
        
        pc_id_ = "PC-" + hostname_;
        
        std::ostringstream oss;
        oss << pc_id_ << "," << username_ << "," << hostname_;
        std::string msg = oss.str();
        
        std::cout << "[Agent] Sending: REGISTER_PC|" << msg << std::endl;
        
        std::string full_msg = "REGISTER_PC|" + msg;
        if (stream_.send(full_msg.c_str(), full_msg.length()) == -1) {
            std::cerr << "[Agent] Failed to send registration" << std::endl;
            return -1;
        }
        
        char buffer[256];
        ssize_t bytes = stream_.recv(buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "[Agent] Registration response: " << buffer << std::endl;
        }
        
        running_ = true;
        listen_for_commands();
        
        return 0;
    }
    
private:
    void listen_for_commands() {
        char buffer[4096];
        while (running_) {
            ssize_t bytes = stream_.recv(buffer, sizeof(buffer) - 1);
            if (bytes <= 0) {
                std::cout << "[Agent] Connection closed" << std::endl;
                break;
            }
            
            buffer[bytes] = '\0';
            std::string command(buffer);
            std::cout << "[Agent] Received command: " << command << std::endl;
            
            if (command.find("LIST_DIR|") == 0) {
                handle_list_dir(command.substr(9));
            }
            else if (command.find("DOWNLOAD_FILE|") == 0) {
                handle_download_file(command.substr(14));
            }
            else if (command.find("HEARTBEAT") == 0) {
                std::string response = "HEARTBEAT_OK\n";
                stream_.send(response.c_str(), response.length());
            }
        }
    }
    
    void handle_list_dir(const std::string& data) {
        size_t pos = data.find('|');
        if (pos == std::string::npos) {
            std::cerr << "[Agent] Invalid LIST_DIR format" << std::endl;
            return;
        }
        
        std::string request_id = data.substr(0, pos);
        std::string path = data.substr(pos + 1);
        
        if (!path.empty() && path.back() == '\n') {
            path.pop_back();
        }
        
        std::cout << "[Agent] Listing directory: " << path << std::endl;
        
        std::ostringstream oss;
        oss << "DIR_LISTING|" << request_id << "|";
        
        DIR* dir = opendir(path.c_str());
        if (dir == nullptr) {
            oss << "ERROR:Cannot open directory\n";
            std::string response = oss.str();
            stream_.send(response.c_str(), response.length());
            return;
        }
        
        struct dirent* entry;
        bool first = true;
        
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            
            std::string fullpath = path + "/" + entry->d_name;
            struct stat st;
            
            if (stat(fullpath.c_str(), &st) == 0) {
                if (!first) oss << ";";
                
                if (S_ISDIR(st.st_mode)) {
                    oss << "DIR:" << entry->d_name;
                } else {
                    oss << "FILE:" << entry->d_name << ":" << st.st_size;
                }
                
                first = false;
            }
        }
        
        closedir(dir);
        oss << "\n";
        
        std::string response = oss.str();
        stream_.send(response.c_str(), response.length());
        std::cout << "[Agent] Sent directory listing" << std::endl;
    }
    
    void handle_download_file(const std::string& data) {
        size_t pos = data.find('|');
        if (pos == std::string::npos) {
            std::cerr << "[Agent] Invalid DOWNLOAD_FILE format" << std::endl;
            return;
        }
        
        std::string request_id = data.substr(0, pos);
        std::string filepath = data.substr(pos + 1);
        
        if (!filepath.empty() && filepath.back() == '\n') {
            filepath.pop_back();
        }
        
        std::cout << "[Agent] Downloading file: " << filepath << std::endl;
        
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::string error = "FILE_DATA|" + request_id + "|ERROR:Cannot open file\n";
            stream_.send(error.c_str(), error.length());
            return;
        }
        
        file.seekg(0, std::ios::end);
        size_t filesize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::ostringstream oss;
        oss << "FILE_DATA|" << request_id << "|" << filesize << "|";
        std::string header = oss.str();
        stream_.send(header.c_str(), header.length());
        
        char buffer[8192];
        while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
            stream_.send(buffer, file.gcount());
        }
        
        file.close();
        std::cout << "[Agent] File sent: " << filesize << " bytes" << std::endl;
    }
    
    ACE_SOCK_Stream stream_;
    std::string relay_host_;
    int relay_port_;
    bool running_;
    std::string pc_id_;
    std::string username_;
    std::string hostname_;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <relay_host> <relay_port>" << std::endl;
        std::cout << "Example: " << argv[0] << " 192.168.100.89 2810" << std::endl;
        return 1;
    }
    
    std::string relay_host = argv[1];
    int relay_port = atoi(argv[2]);
    
    DesktopAgent agent(relay_host, relay_port);
    return agent.connect_to_relay();
}
