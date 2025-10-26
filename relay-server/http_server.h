#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <ace/Task.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>

class FileTransferHandler;

class HTTPServer : public ACE_Task<ACE_MT_SYNCH> {
public:
    HTTPServer(FileTransferHandler* file_handler);
    virtual ~HTTPServer();

    int init(int port);
    virtual int svc();
    void shutdown();

private:
    void handle_request(ACE_SOCK_Stream* client_stream);
    void send_response(ACE_SOCK_Stream* client, int code, const std::string& content_type, 
                      const std::string& body);
    void send_file(ACE_SOCK_Stream* client, const std::string& token);

    ACE_SOCK_Acceptor acceptor_;
    bool running_;
    FileTransferHandler* file_handler_;
};

#endif
