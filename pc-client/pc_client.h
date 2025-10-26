#ifndef PC_CLIENT_H
#define PC_CLIENT_H

#include <ace/Task.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <string>

class PCClient : public ACE_Task<ACE_MT_SYNCH> {
public:
    PCClient(const std::string& pc_id, const std::string& username, const std::string& hostname);
    virtual ~PCClient();

    int connect_to_account_server(const std::string& server_address);
    int register_with_relay(const std::string& relay_address, int port);
    virtual int svc();
    void shutdown();

private:
    std::string pc_id_;
    std::string username_;
    std::string hostname_;
    ACE_SOCK_Stream account_stream_;
    ACE_SOCK_Stream relay_stream_;
    bool running_;
};

#endif
