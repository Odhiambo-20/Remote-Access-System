#include <QCoreApplication>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include "pc_identifier.h"
#include "remote_control_server.h"
#include "file_server.h"

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

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    qDebug() << "========================================";
    qDebug() << "Remote Access PC Client Starting...";
    qDebug() << "========================================";
    
    // Initialize PC identifier
    PCIdentifier identifier;
    QString pcId = identifier.getPCId();
    QString username = identifier.getUsername();
    QString hostname = identifier.getHostname();
    
    qDebug() << "";
    qDebug() << "PC Information:";
    qDebug() << "  ID:" << pcId;
    qDebug() << "  Username:" << username;
    qDebug() << "  Hostname:" << hostname;
    qDebug() << "";
    
    // Start Remote Control Server
    RemoteControlServer remoteServer;
    if (!remoteServer.start(2812)) {
        qDebug() << "Failed to start remote control server";
        return 1;
    }
    
    // Start File Server
    FileServer fileServer;
    if (!fileServer.start(2811, 8080)) {
        qDebug() << "Failed to start file server";
        return 1;
    }
    
    // Register with relay server
    RelayRegistration *registration = new RelayRegistration(pcId, username, hostname);
    
    // Try to register immediately
    QString relayServer = "127.0.0.1"; // Change to your relay server IP
    registration->registerWithRelay(relayServer, 2810);
    
    // Re-register every 30 seconds to maintain presence
    QTimer *heartbeatTimer = new QTimer(&app);
    QObject::connect(heartbeatTimer, &QTimer::timeout, [registration, relayServer]() {
        registration->registerWithRelay(relayServer, 2810);
    });
    heartbeatTimer->start(30000);
    
    qDebug() << "";
    qDebug() << "========================================";
    qDebug() << "✅ PC Client Ready!";
    qDebug() << "========================================";
    qDebug() << "Services:";
    qDebug() << "  Remote Control: Port 2812";
    qDebug() << "  File Transfer:  Port 2811";
    qDebug() << "  HTTP Sharing:   Port 8080";
    qDebug() << "  Relay Server:   " << relayServer << ":2810";
    qDebug() << "";
    qDebug() << "Waiting for connections...";
    qDebug() << "========================================";
    
    return app.exec();
}

#include "main.moc"
