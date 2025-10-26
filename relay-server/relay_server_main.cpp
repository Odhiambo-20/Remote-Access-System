#include "relay_server.h"
#include <ace/Log_Msg.h>
#include <ace/OS_NS_signal.h>
#include <signal.h>
#include <cstdlib>

RelayServer* g_server = nullptr;

void signal_handler(int signum) {
    ACE_DEBUG((LM_INFO, "\n[RelayServer] Shutting down...\n"));
    if (g_server) {
        g_server->shutdown();
    }
    exit(0);
}

int main(int argc, ACE_TCHAR* argv[]) {
    ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::STDERR);
    ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING | LM_ERROR, ACE_Log_Msg::PROCESS);
    
    ACE_OS::signal(SIGINT, (ACE_SignalHandler) signal_handler);
    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) signal_handler);
    
    int relay_port = 2810;
    int file_port = 2811;
    int http_port = 8080;
    
    if (argc >= 4) {
        relay_port = atoi(argv[1]);
        file_port = atoi(argv[2]);
        http_port = atoi(argv[3]);
    }
    
    ACE_DEBUG((LM_INFO, "Starting Relay Server...\n"));
    
    g_server = new RelayServer();
    
    if (g_server->open(relay_port, file_port, http_port) == -1) {
        ACE_ERROR_RETURN((LM_ERROR, "Failed to initialize relay server\n"), -1);
    }
    
    ACE_DEBUG((LM_INFO, "All servers started successfully\n"));
    ACE_DEBUG((LM_INFO, "- Relay Server: port %d\n", relay_port));
    ACE_DEBUG((LM_INFO, "- File Transfer: port %d\n", file_port));
    ACE_DEBUG((LM_INFO, "- HTTP Server: port %d\n", http_port));
    ACE_DEBUG((LM_INFO, "Press Ctrl+C to shutdown\n\n"));
    
    int result = g_server->run();
    
    delete g_server;
    g_server = nullptr;
    
    return result;
}
