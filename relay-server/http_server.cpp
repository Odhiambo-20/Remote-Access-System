#include "http_server.h"
#include "file_transfer_handler.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_string.h>
#include <sstream>

HTTPServer::HTTPServer(FileTransferHandler* file_handler)
    : running_(false)
    , file_handler_(file_handler) {
}

HTTPServer::~HTTPServer() {
    shutdown();
}

int HTTPServer::init(int port) {
    ACE_INET_Addr addr(port);
    
    if (acceptor_.open(addr, 1) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "Failed to open HTTP server on port %d\n", port), -1);
    }
    
    ACE_DEBUG((LM_INFO, "[HTTP] Server listening on port %d\n", port));
    running_ = true;
    
    return this->activate(THR_NEW_LWP | THR_JOINABLE);
}

int HTTPServer::svc() {
    while (running_) {
        ACE_SOCK_Stream* client_stream = new ACE_SOCK_Stream();
        
        if (acceptor_.accept(*client_stream) == -1) {
            if (running_) {
                ACE_ERROR((LM_ERROR, "[HTTP] Accept failed\n"));
            }
            delete client_stream;
            continue;
        }
        
        ACE_DEBUG((LM_INFO, "[HTTP] New request\n"));
        handle_request(client_stream);
        
        client_stream->close();
        delete client_stream;
    }
    
    return 0;
}

void HTTPServer::handle_request(ACE_SOCK_Stream* client_stream) {
    char buffer[4096];
    ssize_t bytes = client_stream->recv(buffer, sizeof(buffer) - 1);
    
    if (bytes <= 0) {
        return;
    }
    
    buffer[bytes] = '\0';
    std::string request(buffer);
    
    ACE_DEBUG((LM_INFO, "[HTTP] Request: %s\n", request.substr(0, 100).c_str()));
    
    // Parse HTTP request
    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;
    
    if (method == "GET" && path.find("/share/") == 0) {
        // Extract token
        std::string token = path.substr(7);
        size_t query_pos = token.find('?');
        if (query_pos != std::string::npos) {
            token = token.substr(0, query_pos);
        }
        
        ACE_DEBUG((LM_INFO, "[HTTP] Share token: %s\n", token.c_str()));
        send_file(client_stream, token);
    }
    else {
        std::string body = "<html><body><h1>Remote Access File Sharing</h1>"
                          "<p>Invalid request</p></body></html>";
        send_response(client_stream, 404, "text/html", body);
    }
}

void HTTPServer::send_response(ACE_SOCK_Stream* client, int code, 
                               const std::string& content_type, 
                               const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << code << " OK\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;
    
    std::string resp_str = response.str();
    client->send(resp_str.c_str(), resp_str.length());
}

void HTTPServer::send_file(ACE_SOCK_Stream* client, const std::string& token) {
    FileShareInfo* info = file_handler_->get_share_info(token);
    
    if (!info) {
        std::string body = "<html><body><h1>404 Not Found</h1>"
                          "<p>File not found or token expired</p></body></html>";
        send_response(client, 404, "text/html", body);
        return;
    }
    
    ACE_DEBUG((LM_INFO, "[HTTP] Serving file: %s from PC: %s\n", 
              info->file_path.c_str(), info->pc_id.c_str()));
    
    // TODO: Actually fetch and serve the file from the PC
    // For now, send a placeholder response
    std::string body = "<html><body><h1>File Download</h1>"
                      "<p>File: " + info->file_path + "</p>"
                      "<p>From PC: " + info->pc_id + "</p>"
                      "<p>Download functionality will be implemented...</p>"
                      "</body></html>";
    send_response(client, 200, "text/html", body);
}

void HTTPServer::shutdown() {
    running_ = false;
    acceptor_.close();
    this->wait();
}
