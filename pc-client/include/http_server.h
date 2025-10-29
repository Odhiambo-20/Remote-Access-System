#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <string>
#include <vector>

namespace RemoteAccessSystem {
namespace Common {

class HTTPServer {
public:
    HTTPServer();
    ~HTTPServer();
    
    /**
     * Start the HTTP server on the specified address and port
     * @param address The IP address to bind to (e.g., "0.0.0.0")
     * @param port The port number to listen on
     * @return true if server started successfully, false otherwise
     */
    bool Start(const std::string& address, uint16_t port);
    
    /**
     * Stop the HTTP server and close all connections
     */
    void Stop();
    
    /**
     * Get the current connection info formatted for QR code
     * Format: pc_id|username|relay_server|relay_port|auth_token
     * @return Connection info string
     */
    std::string GetConnectionInfo() const;
    
    /**
     * Generate QR code data for the current PC connection info
     * @return Connection info string suitable for QR encoding
     */
    std::string GenerateQRData() const;

private:
    /**
     * Handle incoming HTTP requests
     * Supported endpoints:
     * - GET /qr or /qr.png - Returns QR code as PNG image
     * - GET /share/{token} - Returns shared file
     * @param client The connected client socket
     */
    void HandleRequest(ACE_SOCK_Stream& client);
    
    /**
     * Generate PNG image data for QR code
     * @param data The data to encode in the QR code
     * @param size The size of the QR code in pixels (default: 400)
     * @return Vector containing PNG image bytes
     */
    std::vector<uint8_t> GenerateQRCodePNG(const std::string& data, int size = 400);
    
    /**
     * Send HTTP error response
     * @param client The connected client socket
     * @param code HTTP status code (e.g., 404, 500)
     * @param message Error message
     */
    void SendErrorResponse(ACE_SOCK_Stream& client, int code, const std::string& message);
    
    /**
     * Send file as HTTP response
     * @param client The connected client socket
     * @param file_path Path to the file to send
     */
    void SendFileResponse(ACE_SOCK_Stream& client, const std::string& file_path);
    
    ACE_SOCK_Acceptor acceptor_;
    bool running_;
};

/**
 * Add a file sharing token
 * @param token Unique token for accessing the file
 * @param file_path Full path to the file to be shared
 */
void AddShareToken(const std::string& token, const std::string& file_path);

/**
 * Set PC connection information for QR code generation
 * @param pc_id Unique identifier for this PC
 * @param username Username of the PC owner
 * @param relay_server Relay server address
 * @param relay_port Relay server port
 */
void SetPCInfo(const std::string& pc_id, const std::string& username,
               const std::string& relay_server, uint16_t relay_port);

/**
 * Generate authentication token for secure connections
 * @return Generated authentication token
 */
std::string GenerateAuthToken();

/**
 * Display QR code in terminal (ASCII art)
 * @param data The data to encode in the QR code
 */
void DisplayQRCodeInTerminal(const std::string& data);

} // namespace Common
} // namespace RemoteAccessSystem

// C-style API for external access
extern "C" {
    void http_server_add_share_token(const char* token, const char* path);
    void http_server_set_pc_info(const char* id, const char* user,
                                 const char* server, uint16_t port);
    const char* http_server_get_qr_data();
}

#endif // HTTP_SERVER_H