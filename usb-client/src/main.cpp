#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <chrono>
#include <tao/ORB.h>
#include <tao/corba.h>
#include "usb_manager.h"
#include "config_manager.h"
#include "../../common/include/protocol.h"
#include "../../common/include/crypto.h"

using namespace RemoteAccessSystem;
using namespace RemoteAccessSystem::Common;

int main(int argc, char* argv[]) {
    try {
        ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, ACE_TEXT("USBClient"));
        
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
        
        ConfigManager config_manager("usb_client.conf");
        if (!config_manager.LoadConfig()) {
            ACE_ERROR_RETURN((LM_ERROR, "Failed to load configuration\n"), 1);
        }
        
        USBManager usb_manager;
        std::string usb_id = usb_manager.GenerateUSBID();
        if (usb_id.empty()) {
            ACE_ERROR_RETURN((LM_ERROR, "Failed to generate USB ID\n"), 1);
        }
        
        config_manager.SetUSBID(usb_id);
        
        ACE_SOCK_Connector connector;
        ACE_SOCK_Stream socket;
        ACE_INET_Addr server_addr(config_manager.GetServerAddress().c_str());
        
        if (connector.connect(socket, server_addr) == -1) {
            ACE_ERROR_RETURN((LM_ERROR, "Failed to connect to server\n"), 1);
        }
        
        // Send bind request message
        std::string bind_msg = "BIND|" + usb_id + "\n";
        ssize_t bytes_sent = socket.send(bind_msg.c_str(), bind_msg.length());
        
        if (bytes_sent != (ssize_t)bind_msg.length()) {
            ACE_ERROR_RETURN((LM_ERROR, "Failed to send bind request\n"), 1);
        }
        
        // Receive response
        char buffer[1024];
        ssize_t bytes_received = socket.recv(buffer, sizeof(buffer) - 1);
        
        if (bytes_received <= 0) {
            ACE_ERROR_RETURN((LM_ERROR, "Failed to receive bind response\n"), 1);
        }
        
        buffer[bytes_received] = '\0';
        std::string response(buffer);
        
        if (response.find("SUCCESS") == std::string::npos) {
            ACE_ERROR((LM_ERROR, "Bind request failed: %s\n", response.c_str()));
            return 1;
        }
        
        ACE_DEBUG((LM_INFO, "USB client registered successfully: %s\n", usb_id.c_str()));
        
        socket.close();
        return 0;
        
    } catch (const CORBA::Exception& e) {
        ACE_ERROR((LM_ERROR, "CORBA exception: %s\n", e._name()));
        return 1;
    } catch (const std::exception& e) {
        ACE_ERROR((LM_ERROR, "Exception: %s\n", e.what()));
        return 1;
    }
}
