#include "remote_control_server.h"
#include <QDebug>

RemoteControlServer::RemoteControlServer(QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this)) {
    
    connect(m_server, &QTcpServer::newConnection, this, &RemoteControlServer::handleNewConnection);
}

bool RemoteControlServer::start(int port) {
    if (!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "[RemoteControl] Failed to start on port" << port;
        return false;
    }
    
    qDebug() << "[RemoteControl] Server started on port" << port;
    return true;
}

void RemoteControlServer::handleNewConnection() {
    QTcpSocket *client = m_server->nextPendingConnection();
    qDebug() << "[RemoteControl] New client connected:" << client->peerAddress();
    
    // Send connection established message
    client->write("CONNECTION_ESTABLISHED\n");
    client->flush();
    
    connect(client, &QTcpSocket::readyRead, this, [this, client]() {
        handleClientCommand(client);
    });
    
    connect(client, &QTcpSocket::disconnected, this, [client]() {
        qDebug() << "[RemoteControl] Client disconnected";
        client->deleteLater();
    });
}

void RemoteControlServer::handleClientCommand(QTcpSocket *client) {
    QByteArray data = client->readAll();
    QString command = QString::fromUtf8(data).trimmed();
    
    qDebug() << "[RemoteControl] Command:" << command;
    
    QStringList parts = command.split('|');
    if (parts.isEmpty()) return;
    
    QString cmd = parts[0];
    
    if (cmd == "MOUSE_MOVE" && parts.size() >= 3) {
        int x = parts[1].toInt();
        int y = parts[2].toInt();
        // TODO: Implement actual mouse movement using X11
        qDebug() << "[RemoteControl] Mouse move:" << x << y;
        
    } else if (cmd == "MOUSE_CLICK" && parts.size() >= 2) {
        int button = parts[1].toInt();
        // TODO: Implement actual mouse click using X11
        qDebug() << "[RemoteControl] Mouse click:" << button;
        
    } else if (cmd == "KEY_PRESS" && parts.size() >= 2) {
        int key = parts[1].toInt();
        // TODO: Implement actual key press using X11
        qDebug() << "[RemoteControl] Key press:" << key;
        
    } else if (cmd == "DISCONNECT") {
        qDebug() << "[RemoteControl] Client requested disconnect";
        client->disconnectFromHost();
    }
}
