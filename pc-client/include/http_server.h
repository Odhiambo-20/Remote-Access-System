#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <string>

namespace RemoteAccessSystem {
namespace Common {

class HTTPServer {
public:
    HTTPServer();
    ~HTTPServer();

    // Start the HTTP server for file sharing
    bool Start(const std::string& address, uint16_t port);

    // Stop the HTTP server
    void Stop();

private:
    // Handle incoming HTTP requests
    void HandleRequest(ACE_SOCK_Stream& client);

    ACE_SOCK_Acceptor acceptor_;
    bool running_;
};

// Helper function to add share tokens (called by file_handler)
void AddShareToken(const std::string& token, const std::string& file_path);

} // namespace Common
} // namespace RemoteAccessSystem

#endif // HTTP_SERVER_H