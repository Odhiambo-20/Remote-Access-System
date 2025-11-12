#ifndef RELAY_SERVER_H
#define RELAY_SERVER_H

#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Thread_Mutex.h>
#include <string>
#include <map>
#include <ctime>

struct PCInfo {
    std::string pc_id;
    std::string username;
    std::string hostname;
    ACE_SOCK_Stream* connection;
    ACE_SOCK_Stream* file_connection;
    time_t last_seen;
};

struct PendingRequest {
    ACE_SOCK_Stream* mobile_client;
    std::string request_type;
    std::string request_data;
    time_t timestamp;
};

class RelayServer {
public:
    RelayServer();
    ~RelayServer();

    int open(int relay_port, int file_port, int http_port);
    int run();
    void shutdown();

private:
    bool running_;
    int relay_port_;
    int file_port_;
    int http_port_;
    
    ACE_SOCK_Acceptor relay_acceptor_;
    ACE_SOCK_Acceptor file_acceptor_;
    ACE_Thread_Mutex mutex_;
    
    std::map<std::string, PCInfo> registered_pcs_;
    std::map<std::string, PendingRequest> pending_requests_;

    int handle_client(ACE_SOCK_Stream* client_stream);
    void handle_request(ACE_SOCK_Stream* client_stream, const std::string& request);
    
    void handle_pc_registration(ACE_SOCK_Stream* client, const std::string& data);
    void handle_file_handler_registration(ACE_SOCK_Stream* client, const std::string& data);
    void handle_pc_list_query(ACE_SOCK_Stream* client);
    void handle_list_dir_request(ACE_SOCK_Stream* client, const std::string& data);
    void handle_download_request(ACE_SOCK_Stream* client, const std::string& data);
    void handle_generate_url_request(ACE_SOCK_Stream* client, const std::string& data);
    void handle_delete_request(ACE_SOCK_Stream* client, const std::string& data);
    void handle_rename_request(ACE_SOCK_Stream* client, const std::string& data);
    void handle_copy_request(ACE_SOCK_Stream* client, const std::string& data);
    void handle_upload_request(ACE_SOCK_Stream* client, const std::string& data);
    void handle_pc_response(ACE_SOCK_Stream* pc_client, const std::string& response);
    void handle_file_handler_response(ACE_SOCK_Stream* file_handler, const std::string& response);
};

#endif // RELAY_SERVER_H