#ifndef FILE_TRANSFER_HANDLER_H
#define FILE_TRANSFER_HANDLER_H

#include <ace/Task.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <map>
#include <string>
#include <vector>

struct FileShareInfo {
    std::string pc_id;
    std::string file_path;
    std::string share_token;
    time_t created_at;
};

class FileTransferHandler : public ACE_Task<ACE_MT_SYNCH> {
public:
    FileTransferHandler();
    virtual ~FileTransferHandler();

    int init(int port);
    virtual int svc();
    void shutdown();

    std::string generate_share_url(const std::string& pc_id, const std::string& file_path);
    FileShareInfo* get_share_info(const std::string& token);

private:
    void handle_client(ACE_SOCK_Stream* client_stream);
    void handle_mobile_request(ACE_SOCK_Stream* client, const std::string& pc_id, const std::string& request);
    void handle_pc_response(ACE_SOCK_Stream* client, const std::string& pc_id);
    std::string generate_token();

    ACE_SOCK_Acceptor acceptor_;
    bool running_;
    std::map<std::string, ACE_SOCK_Stream*> pc_connections_;
    std::map<std::string, FileShareInfo> share_tokens_;
    ACE_Thread_Mutex mutex_;
};

#endif
