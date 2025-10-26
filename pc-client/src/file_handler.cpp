#include "file_handler.h"
#include <QTcpSocket>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <QNetworkInterface>
#include <QHostAddress>

FileHandler::FileHandler(QObject *parent)
    : QObject(parent), relaySocket(nullptr), isConnected(false)
{
    qDebug() << "[FileHandler] Initialized";
}

FileHandler::~FileHandler()
{
    cleanup();
}

QString FileHandler::getLocalIPAddress()
{
    // Get all network interfaces
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    
    // Look for IPv4 address that's not localhost
    for (const QHostAddress &address : addresses) {
        // Check if it's IPv4 and not localhost
        if (address.protocol() == QAbstractSocket::IPv4Protocol && 
            !address.isLoopback()) {
            QString ip = address.toString();
            // Prefer 192.168.x.x or 10.x.x.x addresses (common local network ranges)
            if (ip.startsWith("192.168.") || ip.startsWith("10.") || ip.startsWith("172.")) {
                qDebug() << "[FileHandler] Using local IP:" << ip;
                return ip;
            }
        }
    }
    
    // Fallback: return first non-loopback IPv4 address
    for (const QHostAddress &address : addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && 
            !address.isLoopback()) {
            QString ip = address.toString();
            qDebug() << "[FileHandler] Using fallback IP:" << ip;
            return ip;
        }
    }
    
    // Last resort: return localhost (will only work on same machine)
    qDebug() << "[FileHandler] WARNING: Could not find local IP, using localhost";
    return "localhost";
}

void FileHandler::connectToRelay(const QString &host, quint16 port, const QString &id)
{
    pcId = id;
    
    if (relaySocket) {
        delete relaySocket;
    }

    relaySocket = new QTcpSocket(this);
    connect(relaySocket, &QTcpSocket::connected, this, &FileHandler::onRelayConnected);
    connect(relaySocket, &QTcpSocket::disconnected, this, &FileHandler::onRelayDisconnected);
    connect(relaySocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &FileHandler::onRelayError);
    connect(relaySocket, &QTcpSocket::readyRead, this, &FileHandler::onRelayDataReceived);

    qDebug() << "[FileHandler] Connecting to relay server at" << host << ":" << port;
    relaySocket->connectToHost(host, port);
}

void FileHandler::onRelayConnected()
{
    qDebug() << "[FileHandler] Connected to relay server";
    isConnected = true;
    
    QString message = "PC_FILE|" + pcId + "\n";
    relaySocket->write(message.toUtf8());
    relaySocket->flush();
    
    qDebug() << "[FileHandler] Sent PC_FILE identification";
    
    // Start heartbeat timer
    QTimer *heartbeatTimer = new QTimer(this);
    connect(heartbeatTimer, &QTimer::timeout, this, [this]() {
        if (relaySocket && relaySocket->isOpen()) {
            relaySocket->write("HEARTBEAT\n");
            relaySocket->flush();
            qDebug() << "[FileHandler] Heartbeat sent";
        }
    });
    heartbeatTimer->start(30000); // Every 30 seconds
}

void FileHandler::onRelayDisconnected()
{
    qDebug() << "[FileHandler] Disconnected from relay server";
    isConnected = false;
    QTimer::singleShot(5000, this, &FileHandler::reconnect);
}

void FileHandler::onRelayError(QAbstractSocket::SocketError error)
{
    qDebug() << "[FileHandler] Socket error:" << error << relaySocket->errorString();
}

void FileHandler::onRelayDataReceived()
{
    if (!relaySocket) return;

    QByteArray data = relaySocket->readAll();
    QString command = QString::fromUtf8(data).trimmed();
    
    qDebug() << "[FileHandler] Received request:" << command;

    QStringList parts = command.split('|');
    
    if (parts.isEmpty()) return;

    QString action = parts[0];

    if (action == "PONG") {
        qDebug() << "[FileHandler] Received PONG from server";
        return;
    }
    else if (action == "LIST_DIR" && parts.size() >= 3) {
        QString id = parts[1];
        QString path = parts[2];
        handleListDir(path);
    }
    else if (action == "DOWNLOAD" && parts.size() >= 3) {
        QString id = parts[1];
        QString filePath = parts[2];
        handleDownload(filePath);
    }
    else if (action == "UPLOAD" && parts.size() >= 4) {
        QString id = parts[1];
        QString remotePath = parts[2];
        qint64 fileSize = parts[3].toLongLong();
        handleUpload(remotePath, fileSize);
    }
    else if (action == "GENERATE_URL" && parts.size() >= 3) {
        QString id = parts[1];
        QString filePath = parts[2];
        handleGenerateUrl(filePath);
    }
}

void FileHandler::handleListDir(const QString &path)
{
    qDebug() << "[FileHandler] Listing directory:" << path;
    
    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << "[FileHandler] Directory does not exist:" << path;
        sendResponse("ERROR|Directory not found\n");
        return;
    }

    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    
    QString response = "DIR_LIST|";
    for (const QFileInfo &entry : entries) {
        QString type = entry.isDir() ? "dir" : "file";
        qint64 size = entry.size();
        response += entry.fileName() + "|" + type + "|" + QString::number(size) + ";";
    }
    response += "\n";
    
    sendResponse(response);
    qDebug() << "[FileHandler] Sent directory listing";
}

void FileHandler::handleDownload(const QString &filePath)
{
    qDebug() << "[FileHandler] Downloading file:" << filePath;
    
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "[FileHandler] File not found:" << filePath;
        sendResponse("ERROR|File not found\n");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[FileHandler] Cannot open file for reading:" << filePath;
        sendResponse("ERROR|Cannot open file\n");
        return;
    }

    qint64 fileSize = file.size();
    
    QString response = "DOWNLOAD_START|" + QString::number(fileSize) + "\n";
    sendResponse(response);

    const int CHUNK_SIZE = 8192;
    QByteArray buffer;
    
    while (!file.atEnd()) {
        buffer = file.read(CHUNK_SIZE);
        if (relaySocket && relaySocket->isOpen()) {
            relaySocket->write(buffer);
            relaySocket->flush();
        }
    }

    file.close();
    qDebug() << "[FileHandler] File download complete:" << filePath;
}

void FileHandler::handleUpload(const QString &remotePath, qint64 fileSize)
{
    qDebug() << "[FileHandler] Preparing to receive file at:" << remotePath << "Size:" << fileSize << "bytes";
    
    // Ensure directory exists
    QFileInfo fileInfo(remotePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "[FileHandler] Cannot create directory:" << dir.path();
            sendResponse("ERROR|Cannot create directory\n");
            return;
        }
    }
    
    QFile file(remotePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "[FileHandler] Cannot open file for writing:" << remotePath;
        sendResponse("ERROR|Cannot create file\n");
        return;
    }

    // Send ready signal to relay server
    qDebug() << "[FileHandler] Sending UPLOAD_READY signal";
    sendResponse("UPLOAD_READY\n");
    
    // Now receive the file data
    const qint64 CHUNK_SIZE = 8192;
    qint64 bytesReceived = 0;
    qint64 lastProgress = 0;

    qDebug() << "[FileHandler] Starting to receive file data...";
    
    // Set a timeout for receiving data
    QTime timeout;
    timeout.start();
    
    while (bytesReceived < fileSize && relaySocket && relaySocket->isOpen()) {
        // Wait for data to be available
        if (relaySocket->bytesAvailable() > 0 || relaySocket->waitForReadyRead(1000)) {
            QByteArray buffer = relaySocket->read(qMin(CHUNK_SIZE, fileSize - bytesReceived));
            
            if (buffer.isEmpty()) {
                qDebug() << "[FileHandler] Empty buffer received";
                QThread::msleep(10);
                continue;
            }
            
            qint64 written = file.write(buffer);
            if (written == -1) {
                qDebug() << "[FileHandler] Write error:" << file.errorString();
                break;
            }
            
            bytesReceived += written;
            timeout.restart();
            
            // Log progress every 10%
            qint64 progress = (bytesReceived * 100) / fileSize;
            if (progress >= lastProgress + 10 || bytesReceived == fileSize) {
                qDebug() << "[FileHandler] Received:" << bytesReceived << "/" << fileSize 
                         << "bytes (" << progress << "%)";
                lastProgress = progress;
            }
        }
        else if (timeout.elapsed() > 30000) { // 30 second timeout
            qDebug() << "[FileHandler] Timeout waiting for data";
            break;
        }
    }

    file.close();
    
    if (bytesReceived == fileSize) {
        sendResponse("UPLOAD_SUCCESS\n");
        qDebug() << "[FileHandler] File upload complete:" << remotePath << "(" << bytesReceived << "bytes)";
    }
    else {
        qDebug() << "[FileHandler] Upload incomplete: received" << bytesReceived << "of" << fileSize;
        sendResponse("ERROR|Upload incomplete\n");
        // Delete incomplete file
        QFile::remove(remotePath);
    }
}

void FileHandler::handleGenerateUrl(const QString &filePath)
{
    qDebug() << "[FileHandler] Generating share URL for:" << filePath;
    
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "[FileHandler] File not found for sharing:" << filePath;
        sendResponse("ERROR|File not found\n");
        return;
    }

    // Generate random token
    QString token;
    const QString charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 32; ++i) {
        int index = qrand() % charset.length();
        token += charset[index];
    }

    shareTokens[token] = filePath;

    // Get the actual local IP address instead of localhost
    QString localIP = getLocalIPAddress();
    QString response = "SHARE_URL|http://" + localIP + ":8080/share/" + token + "\n";
    sendResponse(response);
    
    qDebug() << "[FileHandler] Generated share URL:" << "http://" + localIP + ":8080/share/" + token;
}

void FileHandler::sendResponse(const QString &response)
{
    if (relaySocket && relaySocket->isOpen()) {
        relaySocket->write(response.toUtf8());
        relaySocket->flush();
    }
}

void FileHandler::reconnect()
{
    if (!isConnected) {
        qDebug() << "[FileHandler] Attempting to reconnect...";
        connectToRelay("127.0.0.1", 2810, pcId);
    }
}

void FileHandler::cleanup()
{
    if (relaySocket) {
        relaySocket->close();
        delete relaySocket;
        relaySocket = nullptr;
    }
    isConnected = false;
}

QString FileHandler::getShareUrl(const QString &token)
{
    return shareTokens.value(token, "");
}