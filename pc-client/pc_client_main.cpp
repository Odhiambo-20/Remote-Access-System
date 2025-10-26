#include "pc_client.h"
#include "file_handler.h"
#include "include/http_server.h"
#include <ace/Log_Msg.h>
#include <ace/OS_main.h>
#include <csignal>

PCClient* g_client = nullptr;
FileHandler* g_file_handler = nullptr;
RemoteAccessSystem::Common::HTTPServer* g_http_server = nullptr;

void signal_handler(int signum) {
    ACE_DEBUG((LM_INFO, "\nShutting down PC client...\n"));
    
    if (g_file_handler) {
        g_file_handler->shutdown();
        delete g_file_handler;
        g_file_handler = nullptr;
    }
    
    if (g_http_server) {
        g_http_server->Stop();
        delete g_http_server;
        g_http_server = nullptr;
    }
    
    if (g_client) {
        g_client->shutdown();
        delete g_client;
        g_client = nullptr;
    }
    
    exit(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR* argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Configuration
    std::string pc_id = "PC-a987adb83f2bf5628a5919409cb5d00f";
    std::string username = "victor";
    std::string hostname = "victor-dell";
    std::string account_server = "127.0.0.1:2809";
    std::string relay_server = "127.0.0.1";
    
    ACE_DEBUG((LM_INFO, "Starting PC Client...\n"));
    ACE_DEBUG((LM_INFO, "PC ID: %s\n", pc_id.c_str()));
    ACE_DEBUG((LM_INFO, "Username: %s\n", username.c_str()));
    ACE_DEBUG((LM_INFO, "Hostname: %s\n", hostname.c_str()));
    
    // Start HTTP server for file sharing FIRST
    g_http_server = new RemoteAccessSystem::Common::HTTPServer();
    if (g_http_server->Start("0.0.0.0", 8080)) {
        ACE_DEBUG((LM_INFO, "[HTTPServer] Started on port 8080\n"));
    } else {
        ACE_ERROR((LM_ERROR, "[HTTPServer] Failed to start on port 8080 (continuing without HTTP sharing)\n"));
        delete g_http_server;
        g_http_server = nullptr;
    }
    
    // Create and initialize PC client
    g_client = new PCClient(pc_id, username, hostname);
    
    if (g_client->connect_to_account_server(account_server) == -1) {
        ACE_ERROR((LM_ERROR, "Failed to connect to account server (continuing anyway)\n"));
    }
    
    if (g_client->register_with_relay(relay_server, 2810) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "Failed to register with relay server\n"), -1);
    }
    
    // CRITICAL FIX: Pass HTTPServer instance to FileHandler
    if (g_http_server) {
        g_file_handler = new FileHandler(pc_id, g_http_server);
        if (g_file_handler->connect_to_relay(relay_server, 2810) == -1) {
            ACE_ERROR((LM_ERROR, "Failed to connect file handler to relay server (continuing without file support)\n"));
            delete g_file_handler;
            g_file_handler = nullptr;
        }
    } else {
        ACE_ERROR((LM_ERROR, "HTTPServer not available, skipping FileHandler initialization\n"));
    }
    
    ACE_DEBUG((LM_INFO, "PC Client started successfully\n"));
    ACE_DEBUG((LM_INFO, "Press Ctrl+C to shutdown\n"));
    
    // Keep main thread alive
    ACE_Thread_Manager::instance()->wait();
    
    // Cleanup (order matters)
    if (g_file_handler) {
        delete g_file_handler;
    }
    if (g_http_server) {
        delete g_http_server;
    }
    if (g_client) {
        delete g_client;
    }
    
    return 0;
}