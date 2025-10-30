#include "file_server.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QDateTime>

FileServer::FileServer(QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this)), m_httpServer(new QTcpServer(this)) {
    
    connect(m_server, &QTcpServer::newConnection, this, &FileServer::handleNewConnection);
    connect(m_httpServer, &QTcpServer::newConnection, this, &FileServer::handleHttpConnection);
}

bool FileServer::start(int port, int httpPort) {
    if (!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "[FileServer] Failed to start on port" << port;
        return false;
    }
    
    if (!m_httpServer->listen(QHostAddress::Any, httpPort)) {
        qDebug() << "[FileServer] Failed to start HTTP server on port" << httpPort;
        return false;
    }
    
    qDebug() << "[FileServer] Started on port" << port;
    qDebug() << "[FileServer] HTTP server started on port" << httpPort;
    return true;
}

void FileServer::handleNewConnection() {
    QTcpSocket *client = m_server->nextPendingConnection();
    qDebug() << "[FileServer] New client connected:" << client->peerAddress();
    
    connect(client, &QTcpSocket::readyRead, this, [this, client]() {
        handleClientData(client);
    });
    
    connect(client, &QTcpSocket::disconnected, client, &QTcpSocket::deleteLater);
}

void FileServer::handleClientData(QTcpSocket *client) {
    QByteArray data = client->readAll();
    QString command = QString::fromUtf8(data).trimmed();
    
    qDebug() << "[FileServer] Command:" << command;
    
    QStringList parts = command.split('|');
    if (parts.isEmpty()) return;
    
    QString cmd = parts[0];
    
    if (cmd == "LIST_DIR" && parts.size() >= 2) {
        listDirectory(client, parts[1]);
        
    } else if (cmd == "DOWNLOAD" && parts.size() >= 2) {
        downloadFile(client, parts[1]);
        
    } else if (cmd == "UPLOAD" && parts.size() >= 3) {
        uploadFile(client, parts[1], parts[2].toLongLong());
        
    } else if (cmd == "DELETE" && parts.size() >= 2) {
        deleteFile(client, parts[1]);
        
    } else if (cmd == "GENERATE_LINK" && parts.size() >= 3) {
        generateShareLink(client, parts[1], parts[2].toInt());
        
    } else {
        client->write("ERROR|Unknown command\n");
    }
}

void FileServer::listDirectory(QTcpSocket *client, const QString &path) {
    QDir dir(path);
    if (!dir.exists()) {
        client->write("ERROR|Directory not found\n");
        return;
    }
    
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    QStringList result;
    
    for (const QFileInfo &info : entries) {
        QString type = info.isDir() ? "DIR" : "FILE";
        QString size = info.isDir() ? "-" : QString::number(info.size()) + " bytes";
        result << QString("%1|%2|%3").arg(info.fileName()).arg(size).arg(type);
    }
    
    QString response = "DIR_LIST|" + result.join(';') + "\n";
    client->write(response.toUtf8());
    client->flush();
    
    qDebug() << "[FileServer] Listed" << entries.size() << "entries in" << path;
}

void FileServer::downloadFile(QTcpSocket *client, const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        client->write("ERROR|Failed to open file\n");
        return;
    }
    
    qint64 fileSize = file.size();
    QString header = QString("DOWNLOAD_SIZE|%1\n").arg(fileSize);
    client->write(header.toUtf8());
    client->flush();
    
    qDebug() << "[FileServer] Downloading" << path << "(" << fileSize << "bytes)";
    
    // Send file in chunks
    qint64 totalSent = 0;
    while (!file.atEnd()) {
        QByteArray chunk = file.read(8192);
        client->write(chunk);
        totalSent += chunk.size();
    }
    
    client->flush();
    file.close();
    
    qDebug() << "[FileServer] Download complete:" << totalSent << "bytes";
}

void FileServer::uploadFile(QTcpSocket *client, const QString &path, qint64 size) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        client->write("ERROR|Failed to create file\n");
        return;
    }
    
    qDebug() << "[FileServer] Receiving upload:" << path << "(" << size << "bytes)";
    
    // Read file data
    qint64 received = 0;
    while (received < size) {
        if (!client->waitForReadyRead(30000)) break;
        
        QByteArray data = client->read(size - received);
        file.write(data);
        received += data.size();
    }
    
    file.close();
    
    if (received == size) {
        client->write("SUCCESS|Upload complete\n");
        qDebug() << "[FileServer] Upload complete:" << received << "bytes";
    } else {
        client->write("ERROR|Upload incomplete\n");
        qDebug() << "[FileServer] Upload failed, received" << received << "of" << size;
    }
}

void FileServer::deleteFile(QTcpSocket *client, const QString &path) {
    QFile file(path);
    if (file.remove()) {
        client->write("SUCCESS|File deleted\n");
        qDebug() << "[FileServer] Deleted:" << path;
    } else {
        client->write("ERROR|Failed to delete file\n");
        qDebug() << "[FileServer] Failed to delete:" << path;
    }
}

void FileServer::generateShareLink(QTcpSocket *client, const QString &path, int expiryHours) {
    QFile file(path);
    if (!file.exists()) {
        client->write("ERROR|File not found\n");
        return;
    }
    
    // Generate unique token
    QString token = QCryptographicHash::hash(
        (path + QDateTime::currentDateTime().toString()).toUtf8(),
        QCryptographicHash::Md5
    ).toHex();
    
    // Store share info
    ShareInfo info;
    info.filePath = path;
    info.expiryTime = QDateTime::currentDateTime().addSecs(expiryHours * 3600);
    m_shareLinks[token] = info;
    
    // Generate URL
    QString url = QString("http://%1:%2/share/%3")
        .arg(getLocalIP())
        .arg(m_httpServer->serverPort())
        .arg(token);
    
    QString response = QString("SHARE_LINK|%1\n").arg(url);
    client->write(response.toUtf8());
    client->flush();
    
    qDebug() << "[FileServer] Generated share link:" << url;
    qDebug() << "[FileServer] Expires:" << info.expiryTime.toString();
}

void FileServer::handleHttpConnection() {
    QTcpSocket *client = m_httpServer->nextPendingConnection();
    
    connect(client, &QTcpSocket::readyRead, this, [this, client]() {
        QByteArray request = client->readAll();
        QString requestStr = QString::fromUtf8(request);
        
        qDebug() << "[FileServer] HTTP request:" << requestStr.left(100);
        
        // Parse HTTP GET request
        QStringList lines = requestStr.split('\n');
        if (lines.isEmpty()) {
            client->disconnectFromHost();
            return;
        }
        
        QString requestLine = lines[0];
        QStringList parts = requestLine.split(' ');
        if (parts.size() < 2) {
            client->disconnectFromHost();
            return;
        }
        
        QString path = parts[1];
        
        // Handle share link: /share/TOKEN
        if (path.startsWith("/share/")) {
            QString token = path.mid(7);
            serveSharedFile(client, token);
        } else {
            // 404 Not Found
            QString response = "HTTP/1.1 404 Not Found\r\n\r\n";
            client->write(response.toUtf8());
            client->flush();
            client->disconnectFromHost();
        }
    });
    
    connect(client, &QTcpSocket::disconnected, client, &QTcpSocket::deleteLater);
}

void FileServer::serveSharedFile(QTcpSocket *client, const QString &token) {
    if (!m_shareLinks.contains(token)) {
        QString response = "HTTP/1.1 404 Not Found\r\n\r\nShare link not found or expired";
        client->write(response.toUtf8());
        client->flush();
        client->disconnectFromHost();
        return;
    }
    
    ShareInfo info = m_shareLinks[token];
    
    // Check expiry
    if (QDateTime::currentDateTime() > info.expiryTime) {
        m_shareLinks.remove(token);
        QString response = "HTTP/1.1 410 Gone\r\n\r\nShare link has expired";
        client->write(response.toUtf8());
        client->flush();
        client->disconnectFromHost();
        return;
    }
    
    // Serve file
    QFile file(info.filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString response = "HTTP/1.1 500 Internal Server Error\r\n\r\nFailed to open file";
        client->write(response.toUtf8());
        client->flush();
        client->disconnectFromHost();
        return;
    }
    
    QFileInfo fileInfo(info.filePath);
    QString fileName = fileInfo.fileName();
    qint64 fileSize = file.size();
    
    // Send HTTP response headers
    QString headers = QString(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Disposition: attachment; filename=\"%1\"\r\n"
        "Content-Length: %2\r\n"
        "\r\n"
    ).arg(fileName).arg(fileSize);
    
    client->write(headers.toUtf8());
    
    // Send file data
    while (!file.atEnd()) {
        QByteArray chunk = file.read(8192);
        client->write(chunk);
    }
    
    client->flush();
    file.close();
    
    qDebug() << "[FileServer] Served shared file:" << info.filePath << "(" << fileSize << "bytes)";
    
    client->disconnectFromHost();
}

QString FileServer::getLocalIP() {
    // Return local IP address (simplified)
    return "192.168.1.100"; // TODO: Implement actual IP detection
}