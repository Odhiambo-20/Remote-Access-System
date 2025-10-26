#include "file_transfer_handler.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>

FileTransferHandler::FileTransferHandler(QObject *parent)
    : QObject(parent), server(nullptr), isListening(false)
{
    qDebug() << "[FileTransfer] Initialized";
}

FileTransferHandler::~FileTransferHandler()
{
    cleanup();
}

void FileTransferHandler::start(quint16 port)
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &FileTransferHandler::onNewConnection);

    if (server->listen(QHostAddress::Any, port)) {
        isListening = true;
        qDebug() << "[FileTransfer] Server listening on port" << port;
    }
    else {
        qDebug() << "[FileTransfer] Failed to listen on port" << port;
    }
}

void FileTransferHandler::onNewConnection()
{
    while (server->hasPendingConnections()) {
        QTcpSocket *socket = server->nextPendingConnection();
        qDebug() << "[FileTransfer] New connection from" << socket->peerAddress().toString();

        RelaySession *session = new RelaySession(this);
        session->setClientSocket(socket);

        connect(socket, &QTcpSocket::disconnected, session, &RelaySession::onDisconnected);
        sessions.append(session);
        qDebug() << "[FileTransfer] Total sessions:" << sessions.size();
    }
}

void FileTransferHandler::registerPCHandler(const QString &pcId, QTcpSocket *socket)
{
    qDebug() << "[FileTransfer] PC Handler registered for PC:" << pcId;
    pcHandlers[pcId] = socket;

    connect(socket, &QTcpSocket::disconnected, this, [this, pcId]() {
        qDebug() << "[FileTransfer] PC Handler disconnected for PC:" << pcId;
        pcHandlers.remove(pcId);
    });
}

void FileTransferHandler::routeToPC(const QString &pcId, const QString &command, QTcpSocket *sourceSocket)
{
    if (pcHandlers.contains(pcId)) {
        QTcpSocket *pcSocket = pcHandlers[pcId];
        if (pcSocket && pcSocket->isOpen()) {
            qDebug() << "[FileTransfer] Routing to PC:" << pcId;
            pcSocket->write(command.toUtf8());
            pcSocket->flush();
            sourceToPC[sourceSocket] = pcSocket;
            pcToSource[pcSocket] = sourceSocket;
            connect(pcSocket, &QTcpSocket::readyRead, this, &FileTransferHandler::onPCResponse);
        }
    }
}

void FileTransferHandler::onPCResponse()
{
    QTcpSocket *pcSocket = qobject_cast<QTcpSocket *>(sender());
    if (!pcSocket || !pcToSource.contains(pcSocket)) return;

    QTcpSocket *sourceSocket = pcToSource[pcSocket];
    if (sourceSocket && sourceSocket->isOpen()) {
        QByteArray data = pcSocket->readAll();
        sourceSocket->write(data);
        sourceSocket->flush();
    }
}

void FileTransferHandler::cleanup()
{
    if (server) {
        server->close();
    }
}

RelaySession::RelaySession(FileTransferHandler *handler, QObject *parent)
    : QObject(parent), fileHandler(handler), clientSocket(nullptr), pcSocket(nullptr), 
      isDownloading(false), isUploading(false), bytesRemaining(0)
{
}

RelaySession::~RelaySession()
{
    if (clientSocket) {
        clientSocket->deleteLater();
    }
}

void RelaySession::setClientSocket(QTcpSocket *socket)
{
    clientSocket = socket;
    connect(clientSocket, &QTcpSocket::readyRead, this, &RelaySession::onClientData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &RelaySession::onDisconnected);
    connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &RelaySession::onError);
}

void RelaySession::onClientData()
{
    if (!clientSocket || !clientSocket->isOpen()) return;

    QByteArray data = clientSocket->readAll();
    QString command = QString::fromUtf8(data).trimmed();

    qDebug() << "[FileTransfer] Session received:" << command;

    QStringList parts = command.split('|');
    if (parts.isEmpty()) return;

    QString action = parts[0];

    if (action == "PC_FILE" && parts.size() >= 2) {
        QString pcId = parts[1];
        fileHandler->registerPCHandler(pcId, clientSocket);
        pcSocket = clientSocket;
    }
    else if (action == "LIST_DIR" && parts.size() >= 3) {
        QString pcId = parts[1];
        QString path = parts[2];
        QString cmd = "LIST_DIR|" + pcId + "|" + path;
        fileHandler->routeToPC(pcId, cmd, clientSocket);
    }
    else if (action == "DOWNLOAD" && parts.size() >= 3) {
        QString pcId = parts[1];
        QString filePath = parts[2];
        QString cmd = "DOWNLOAD|" + pcId + "|" + filePath;
        fileHandler->routeToPC(pcId, cmd, clientSocket);
        isDownloading = true;
    }
    else if (isDownloading || isUploading) {
        if (pcSocket && pcSocket->isOpen() && pcSocket != clientSocket) {
            pcSocket->write(data);
            pcSocket->flush();
        }
    }
}

void RelaySession::onDisconnected()
{
    qDebug() << "[FileTransfer] Session disconnected";
    if (clientSocket) {
        clientSocket->deleteLater();
        clientSocket = nullptr;
    }
    deleteLater();
}

void RelaySession::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "[FileTransfer] Session error:" << error;
}
