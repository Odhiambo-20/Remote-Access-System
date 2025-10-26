#include "pcmanager.h"
#include <QDebug>
#include <QStringList>

PCManager::PCManager(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_isConnected(false)
{
    connect(m_socket, &QTcpSocket::connected, this, &PCManager::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &PCManager::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &PCManager::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &PCManager::onError);
}

PCManager::~PCManager() {
    if (m_socket->isOpen()) {
        m_socket->close();
    }
}

void PCManager::queryPCList(const QString &relayServerAddress, int port) {
    qDebug() << "[PCManager] Querying PC list from" << relayServerAddress << ":" << port;
    
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected(500);
    }
    
    m_socket->connectToHost(relayServerAddress, port);
}

void PCManager::connectToPC(const QString &pcId, const QString &relayServerAddress) {
    qDebug() << "[PCManager] Connecting to PC:" << pcId;
    
    m_currentPcId = pcId;
    m_connectedPcId = pcId;
    m_isConnected = true;
    
    emit isConnectedChanged();
    emit connectedPcIdChanged();
    
    // Emit connection established signal so FileManager can connect
    emit connectionEstablished(pcId);
    
    qDebug() << "[PCManager] Connection established signal emitted for:" << pcId;
}

void PCManager::disconnectFromPC() {
    qDebug() << "[PCManager] Disconnecting from PC:" << m_connectedPcId;
    
    m_isConnected = false;
    m_connectedPcId = "";
    
    emit isConnectedChanged();
    emit connectedPcIdChanged();
}

void PCManager::onConnected() {
    qDebug() << "[PCManager] Socket connected to relay server";
    // Changed from QUERY_PC_LIST to GET_PCS to match relay server
    QString request = "GET_PCS\n";
    qDebug() << "[PCManager] Sending:" << request.trimmed();
    m_socket->write(request.toUtf8());
    m_socket->flush();
}

void PCManager::onReadyRead() {
    QByteArray data = m_socket->readAll();
    QString response = QString::fromUtf8(data).trimmed();
    qDebug() << "[PCManager] Response:" << response;
    
    if (response.startsWith("PC_LIST|")) {
        emit clearPCList();
        
        // Extract content after "PC_LIST|"
        QString content = response.mid(8);
        
        // Split by semicolon to get individual PCs
        QStringList pcEntries = content.split(';', Qt::SkipEmptyParts);
        QStringList pcList;
        
        qDebug() << "[PCManager] Found" << pcEntries.size() << "PC entries";
        
        for (const QString &entry : pcEntries) {
            // Each entry is: pc_id,username,pc_name
            QStringList parts = entry.split(',');
            if (parts.size() >= 3) {
                QString pcId = parts[0].trimmed();
                QString username = parts[1].trimmed();
                QString hostname = parts[2].trimmed();
                
                pcList.append(pcId);
                emit addPCToList(pcId, hostname, username, true);
                qDebug() << "[PCManager] Added PC:" << hostname << "(" << username << ")";
            }
        }
        
        emit pcListUpdated(pcList);
        qDebug() << "[PCManager] PC list updated with" << pcList.size() << "PCs";
    } else if (response.startsWith("ERROR|")) {
        qDebug() << "[PCManager] Error from relay:" << response;
        emit connectionFailed(response);
    }
    
    m_socket->disconnectFromHost();
}

void PCManager::onDisconnected() {
    qDebug() << "[PCManager] Disconnected from relay server";
}

void PCManager::onError(QAbstractSocket::SocketError error) {
    qDebug() << "[PCManager] Socket error:" << m_socket->errorString();
    emit connectionFailed(m_socket->errorString());
}