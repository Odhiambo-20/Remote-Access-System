#include "remote_control_client.h"
#include <QDebug>

RemoteControlClient::RemoteControlClient(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this)), m_connected(false) {
    
    connect(m_socket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "[RemoteControlClient] Connected to PC";
        m_connected = true;
        emit connected();
    });
    
    connect(m_socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "[RemoteControlClient] Disconnected from PC";
        m_connected = false;
        emit disconnected();
    });
    
    connect(m_socket, &QTcpSocket::readyRead, this, &RemoteControlClient::handleData);
    
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError socketError) {
        qDebug() << "[RemoteControlClient] Socket error:" << m_socket->errorString();
        emit error(m_socket->errorString());
    });
}

void RemoteControlClient::connectToPC(const QString &pcId, const QString &relayServer, int relayPort) {
    qDebug() << "[RemoteControlClient] Requesting connection to PC:" << pcId;
    qDebug() << "[RemoteControlClient] Via relay:" << relayServer << ":" << relayPort;
    
    // Step 1: Connect to relay server
    m_socket->connectToHost(relayServer, relayPort);
    
    if (m_socket->waitForConnected(5000)) {
        // Step 2: Request connection to specific PC
        QString request = QString("CONNECT_TO_PC|%1\n").arg(pcId);
        m_socket->write(request.toUtf8());
        m_socket->flush();
        qDebug() << "[RemoteControlClient] Sent connection request:" << request.trimmed();
    } else {
        qDebug() << "[RemoteControlClient] Failed to connect to relay server";
        emit error("Failed to connect to relay server");
    }
}

void RemoteControlClient::disconnect() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write("DISCONNECT\n");
        m_socket->flush();
        m_socket->disconnectFromHost();
    }
}

void RemoteControlClient::sendMouseMove(int x, int y) {
    if (!m_connected) return;
    QString cmd = QString("MOUSE_MOVE|%1|%2\n").arg(x).arg(y);
    m_socket->write(cmd.toUtf8());
}

void RemoteControlClient::sendMouseClick(int button) {
    if (!m_connected) return;
    QString cmd = QString("MOUSE_CLICK|%1\n").arg(button);
    m_socket->write(cmd.toUtf8());
}

void RemoteControlClient::sendKeyPress(int key) {
    if (!m_connected) return;
    QString cmd = QString("KEY_PRESS|%1\n").arg(key);
    m_socket->write(cmd.toUtf8());
}

void RemoteControlClient::handleData() {
    QByteArray data = m_socket->readAll();
    qDebug() << "[RemoteControlClient] Received:" << data.trimmed();
    
    QString message = QString::fromUtf8(data).trimmed();
    
    if (message.startsWith("CONNECTION_ESTABLISHED")) {
        qDebug() << "[RemoteControlClient] Remote control session established";
        emit sessionStarted();
    } else if (message.startsWith("SCREEN_FRAME|")) {
        // Handle screen frame data
        emit frameReceived(data);
    } else if (message.startsWith("ERROR|")) {
        QString errorMsg = message.mid(6);
        emit error(errorMsg);
    }
}

bool RemoteControlClient::isConnected() const {
    return m_connected;
}
