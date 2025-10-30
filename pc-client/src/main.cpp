#include <QCoreApplication>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include <iostream>
#include "pc_identifier.h"
#include "remote_control_server.h"
#include "file_server.h"
#include "http_server.h"
#include "file_handler.h"  // ADD THIS

class RelayRegistration : public QObject {
    Q_OBJECT
    
public:
    RelayRegistration(const QString &pcId, const QString &username, const QString &hostname) 
        : m_pcId(pcId), m_username(username), m_hostname(hostname), m_socket(new QTcpSocket(this)) {
        connect(m_socket, &QTcpSocket::connected, this, &RelayRegistration::onConnected);
        connect(m_socket, &QTcpSocket::readyRead, this, &RelayRegistration::onReadyRead);
        connect(m_socket, &QTcpSocket::errorOccurred, this, &RelayRegistration::onError);
    }
    
    void registerWithRelay(const QString &relayServer, int port) {
        qDebug() << "[PCClient] Connecting to relay server" << relayServer << ":" << port;
        m_socket->connectToHost(relayServer, port);
    }
    
private slots:
    void onConnected() {
        qDebug() << "[PCClient] Connected to relay server";
        QString message = QString("REGISTER|%1|%2|%3\n")
            .arg(m_pcId)
            .arg(m_username)
            .arg(m_hostname);
        
        qDebug() << "[PCClient] Sending:" << message.trimmed();
        m_socket->write(message.toUtf8());
        m_socket->flush();
    }
    
    void onReadyRead() {
        QByteArray data = m_socket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[PCClient] Relay response:" << response;
        
        if (response.startsWith("OK|")) {
            qDebug() << "✅ Registration successful!";
        } else if (response.startsWith("ERROR|")) {
            qDebug() << "❌ Registration failed:" << response;
        }
        
        m_socket->disconnectFromHost();
    }
    
    void onError(QAbstractSocket::SocketError error) {
        qDebug() << "[PCClient] Connection error:" << m_socket->errorString();
    }
    
private:
    QString m_pcId;
    QString m_username;
    QString m_hostname;
    QTcpSocket *m_socket;
};

// Function to get local IP address
QString getLocalIPAddress() {
    QTcpSocket socket;
    socket.connectToHost("8.8.8.8", 53); // Connect to Google DNS
    if (socket.waitForConnected(1000)) {
        return socket.localAddress().toString();
    }
    return "127.0.0.1";
}

// Function to print colored banner
void printBanner() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                            ║\n";
    std::cout << "║          🖥️  REMOTE ACCESS PC CLIENT v1.0 🖥️              ║\n";
    std::cout << "║                                                            ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

// Function to print QR code instructions
void printQRInstructions(const QString& httpUrl) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   📱 MOBILE APP SETUP 📱                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "To connect your mobile device:\n";
    std::cout << "\n";
    std::cout << "1️⃣  Open the Remote Access mobile app\n";
    std::cout << "2️⃣  Tap 'Scan QR Code'\n";
    std::cout << "3️⃣  Scan the QR code displayed below\n";
    std::cout << "\n";
    std::cout << "OR\n";
    std::cout << "\n";
    std::cout << "🌐 Open this URL in your mobile browser:\n";
    std::cout << "   " << httpUrl.toStdString() << "\n";
    std::cout << "\n";
    std::cout << "════════════════════════════════════════════════════════════\n";
    std::cout << "\n";
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    // Print banner
    printBanner();
    
    qDebug() << "🚀 Remote Access PC Client Starting...";
    qDebug() << "";
    
    // Initialize PC identifier
    PCIdentifier identifier;
    QString pcId = identifier.getPCId();
    QString username = identifier.getUsername();
    QString hostname = identifier.getHostname();
    QString localIP = getLocalIPAddress();
    
    qDebug() << "📋 PC Information:";
    qDebug() << "   ID:       " << pcId;
    qDebug() << "   Username: " << username;
    qDebug() << "   Hostname: " << hostname;
    qDebug() << "   Local IP: " << localIP;
    qDebug() << "";
    
    // Configure relay server
    QString relayServer = "127.0.0.1"; // Change to your relay server IP
    uint16_t relayPort = 2810;
    
    // Set PC info for HTTP server and QR generation
    RemoteAccessSystem::Common::SetPCInfo(
        pcId.toStdString(),
        username.toStdString(),
        relayServer.toStdString(),
        relayPort
    );
    
    // Start HTTP server for QR code and file sharing
    qDebug() << "🌐 Starting HTTP Server...";
    RemoteAccessSystem::Common::HTTPServer httpServer;
    if (!httpServer.Start("0.0.0.0", 8080)) {
        qDebug() << "❌ Failed to start HTTP server";
        return 1;
    }
    qDebug() << "✅ HTTP Server started on port 8080";
    qDebug() << "";
    
    // Start Remote Control Server
    qDebug() << "🎮 Starting Remote Control Server...";
    RemoteControlServer remoteServer;
    if (!remoteServer.start(2812)) {
        qDebug() << "❌ Failed to start remote control server";
        return 1;
    }
    qDebug() << "✅ Remote Control Server started on port 2812";
    qDebug() << "";
    
    // Start File Server (try different HTTP ports until one works)
    qDebug() << "📁 Starting File Server...";
    FileServer fileServer;
    int fileHttpPort = 8081;
    bool fileServerStarted = false;

    for (int port = 8082; port <= 8099; port++) {
        if (fileServer.start(2811, port)) {
            fileHttpPort = port;
            fileServerStarted = true;
            qDebug() << "✅ File Server started on port 2811";
            qDebug() << "✅ File Server HTTP on port" << port;
            break;
        }
    }

    if (!fileServerStarted) {
        qDebug() << "❌ Failed to start file server - no available HTTP ports";
        return 1;
    }
    qDebug() << "";
    
    // ============================================================
    // CRITICAL FIX: Initialize FileHandler for relay communication
    // ============================================================
    qDebug() << "📂 Starting File Handler...";
    FileHandler* fileHandler = new FileHandler(pcId.toStdString(), &httpServer);
    
    if (fileHandler->connect_to_relay(relayServer.toStdString(), relayPort) == 0) {
        qDebug() << "✅ File Handler connected to relay server";
    } else {
        qDebug() << "❌ Failed to connect File Handler to relay server";
        qDebug() << "⚠️  File operations will not work!";
    }
    qDebug() << "";
    // ============================================================
    
    // Register with relay server
    qDebug() << "🔗 Registering with Relay Server...";
    RelayRegistration *registration = new RelayRegistration(pcId, username, hostname);
    
    // Try to register immediately
    registration->registerWithRelay(relayServer, relayPort);
    
    // Re-register every 30 seconds to maintain presence
    QTimer *heartbeatTimer = new QTimer(&app);
    QObject::connect(heartbeatTimer, &QTimer::timeout, [registration, relayServer, relayPort]() {
        registration->registerWithRelay(relayServer, relayPort);
    });
    heartbeatTimer->start(30000);
    
    qDebug() << "";
    qDebug() << "════════════════════════════════════════════════════════════";
    qDebug() << "✅ PC Client Ready and Running!";
    qDebug() << "════════════════════════════════════════════════════════════";
    qDebug() << "";
    qDebug() << "📡 Services Active:";
    qDebug() << "   • Remote Control: Port 2812";
    qDebug() << "   • File Transfer:  Port 2811";
    qDebug() << "   • HTTP Server:    Port 8080 (QR Code)";
    qDebug() << "   • File HTTP:      Port" << fileHttpPort << "(File Sharing)";
    qDebug() << "   • File Handler:   Connected to Relay";
    qDebug() << "   • Relay Server:   " << relayServer << ":" << relayPort;
    qDebug() << "";
    
    // Display QR code instructions
    QString httpUrl = QString("http://%1:8080/qr.png").arg(localIP);
    printQRInstructions(httpUrl);
    
    // Display QR code in terminal
    std::string qrData = httpServer.GetConnectionInfo();
    std::cout << "🔲 Scan this QR Code with your mobile app:\n\n";
    RemoteAccessSystem::Common::DisplayQRCodeInTerminal(qrData);
    
    std::cout << "\n";
    std::cout << "🔐 Connection Data:\n";
    std::cout << "   " << qrData << "\n";
    std::cout << "\n";
    std::cout << "════════════════════════════════════════════════════════════\n";
    std::cout << "\n";
    
    qDebug() << "⏳ Waiting for mobile connections...";
    qDebug() << "💡 Press Ctrl+C to stop the server";
    qDebug() << "";
    
    // Setup signal handling for graceful shutdown
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&]() {
        qDebug() << "\n🛑 Shutting down PC Client...";
        fileHandler->shutdown();
        delete fileHandler;
        httpServer.Stop();
        qDebug() << "✅ Cleanup complete. Goodbye!";
    });
    
    return app.exec();
}

#include "main.moc"