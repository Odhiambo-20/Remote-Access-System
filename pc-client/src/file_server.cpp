#include "file_server.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QDateTime>
#include <QNetworkInterface>

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

// ADD THIS NEW METHOD IMPLEMENTATION
void FileServer::addShareToken(const QString& token, const QString& filePath, int expiryHours) {
    qDebug() << "[FileServer] Adding share token:" << token << "for file:" << filePath;
    
    ShareInfo info;
    info.filePath = filePath;
    info.expiryTime = QDateTime::currentDateTime().addSecs(expiryHours * 3600);
    m_shareLinks[token] = info;
    
    qDebug() << "[FileServer] Token added, expires:" << info.expiryTime.toString();
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
    
    qDebug() << "[FileServer] Command received:" << command;
    
    QStringList parts = command.split('|');
    if (parts.isEmpty()) {
        qDebug() << "[FileServer] Empty command received";
        return;
    }
    
    QString cmd = parts[0];
    
    // Handle LIST command (mobile app sends this)
    if (cmd == "LIST" && parts.size() >= 2) {
        qDebug() << "[FileServer] Processing LIST command for path:" << parts[1];
        listDirectory(client, parts[1]);
        
    // Also support LIST_DIR for backward compatibility
    } else if (cmd == "LIST_DIR" && parts.size() >= 2) {
        qDebug() << "[FileServer] Processing LIST_DIR command for path:" << parts[1];
        listDirectory(client, parts[1]);
        
    } else if (cmd == "DOWNLOAD" && parts.size() >= 2) {
        qDebug() << "[FileServer] Processing DOWNLOAD command for:" << parts[1];
        downloadFile(client, parts[1]);
        
    } else if (cmd == "GET" && parts.size() >= 2) {
        qDebug() << "[FileServer] Processing GET command for:" << parts[1];
        downloadFile(client, parts[1]);
        
    } else if (cmd == "UPLOAD" && parts.size() >= 3) {
        qDebug() << "[FileServer] Processing UPLOAD command";
        uploadFile(client, parts[1], parts[2].toLongLong());
        
    } else if (cmd == "PUT" && parts.size() >= 3) {
        qDebug() << "[FileServer] Processing PUT command";
        uploadFile(client, parts[1], parts[2].toLongLong());
        
    } else if (cmd == "DELETE" && parts.size() >= 2) {
        qDebug() << "[FileServer] Processing DELETE command";
        deleteFile(client, parts[1]);
        
    } else if (cmd == "MKDIR" && parts.size() >= 2) {
        qDebug() << "[FileServer] Processing MKDIR command";
        createDirectory(client, parts[1]);
        
    } else if (cmd == "GENERATE_LINK" && parts.size() >= 3) {
        qDebug() << "[FileServer] Processing GENERATE_LINK command";
        generateShareLink(client, parts[1], parts[2].toInt());
        
    } else {
        qDebug() << "[FileServer] Unknown command:" << cmd << "with" << parts.size() << "parts";
        client->write("ERROR|Unknown command\n");
        client->flush();
    }
}

void FileServer::listDirectory(QTcpSocket *client, const QString &path) {
    qDebug() << "[FileServer] Listing directory:" << path;
    
    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << "[FileServer] Directory does not exist:" << path;
        client->write("ERROR|Directory not found\n");
        client->flush();
        return;
    }
    
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    QStringList result;
    
    for (const QFileInfo &info : entries) {
        QString type = info.isDir() ? "DIR" : "FILE";
        QString size = info.isDir() ? "0" : QString::number(info.size());
        
        // Format: filename,type,size
        QString entry = QString("%1,%2,%3").arg(info.fileName()).arg(type).arg(size);
        result << entry;
        
        qDebug() << "[FileServer]   -" << entry;
    }
    
    // Response format: FILE_LIST|path|file1,type,size;file2,type,size;...
    QString response = QString("FILE_LIST|%1|%2\n").arg(path).arg(result.join(';'));
    client->write(response.toUtf8());
    client->flush();
    
    qDebug() << "[FileServer] Sent" << entries.size() << "entries for" << path;
}

void FileServer::downloadFile(QTcpSocket *client, const QString &path) {
    qDebug() << "[FileServer] Download requested:" << path;
    
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[FileServer] Failed to open file:" << path;
        client->write("ERROR|Failed to open file\n");
        client->flush();
        return;
    }
    
    qint64 fileSize = file.size();
    QString header = QString("FILE_DATA|%1\n").arg(fileSize);
    client->write(header.toUtf8());
    client->flush();
    
    qDebug() << "[FileServer] Sending file:" << path << "(" << fileSize << "bytes)";
    
    // Send file in chunks
    qint64 totalSent = 0;
    while (!file.atEnd()) {
        QByteArray chunk = file.read(8192);
        qint64 sent = client->write(chunk);
        totalSent += sent;
        client->flush();
    }
    
    file.close();
    
    qDebug() << "[FileServer] Download complete:" << totalSent << "of" << fileSize << "bytes sent";
}

void FileServer::uploadFile(QTcpSocket *client, const QString &path, qint64 size) {
    qDebug() << "[FileServer] Upload requested:" << path << "(" << size << "bytes)";
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "[FileServer] Failed to create file:" << path;
        client->write("ERROR|Failed to create file\n");
        client->flush();
        return;
    }
    
    // Acknowledge ready to receive
    client->write("READY\n");
    client->flush();
    
    // Read file data
    qint64 received = 0;
    while (received < size) {
        if (!client->waitForReadyRead(30000)) {
            qDebug() << "[FileServer] Timeout waiting for data";
            break;
        }
        
        QByteArray data = client->read(qMin(8192LL, size - received));
        file.write(data);
        received += data.size();
        
        if (received % (1024 * 1024) == 0) {
            qDebug() << "[FileServer] Upload progress:" << received << "/" << size;
        }
    }
    
    file.close();
    
    if (received == size) {
        client->write("OK|Upload complete\n");
        client->flush();
        qDebug() << "[FileServer] Upload complete:" << received << "bytes";
    } else {
        client->write("ERROR|Upload incomplete\n");
        client->flush();
        qDebug() << "[FileServer] Upload failed, received" << received << "of" << size;
    }
}

void FileServer::deleteFile(QTcpSocket *client, const QString &path) {
    qDebug() << "[FileServer] Delete requested:" << path;
    
    QFileInfo info(path);
    
    bool success = false;
    if (info.isDir()) {
        QDir dir(path);
        success = dir.removeRecursively();
    } else {
        QFile file(path);
        success = file.remove();
    }
    
    if (success) {
        client->write("OK|File deleted\n");
        client->flush();
        qDebug() << "[FileServer] Deleted:" << path;
    } else {
        client->write("ERROR|Failed to delete file\n");
        client->flush();
        qDebug() << "[FileServer] Failed to delete:" << path;
    }
}

void FileServer::createDirectory(QTcpSocket *client, const QString &path) {
    qDebug() << "[FileServer] Create directory requested:" << path;
    
    QDir dir;
    if (dir.mkpath(path)) {
        client->write("OK|Directory created\n");
        client->flush();
        qDebug() << "[FileServer] Created directory:" << path;
    } else {
        client->write("ERROR|Failed to create directory\n");
        client->flush();
        qDebug() << "[FileServer] Failed to create directory:" << path;
    }
}

void FileServer::generateShareLink(QTcpSocket *client, const QString &path, int expiryHours) {
    qDebug() << "[FileServer] Generate share link:" << path << "expiry:" << expiryHours << "hours";
    
    QFile file(path);
    if (!file.exists()) {
        qDebug() << "[FileServer] File not found for share link:" << path;
        client->write("ERROR|File not found\n");
        client->flush();
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
    qDebug() << "[FileServer] Serving shared file, token:" << token;
    
    if (!m_shareLinks.contains(token)) {
        qDebug() << "[FileServer] Share link not found:" << token;
        QString response = "HTTP/1.1 404 Not Found\r\n\r\nShare link not found or expired";
        client->write(response.toUtf8());
        client->flush();
        client->disconnectFromHost();
        return;
    }
    
    ShareInfo info = m_shareLinks[token];
    
    // Check expiry
    if (QDateTime::currentDateTime() > info.expiryTime) {
        qDebug() << "[FileServer] Share link expired:" << token;
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
        qDebug() << "[FileServer] Failed to open shared file:" << info.filePath;
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
    // Get the first non-localhost IPv4 address
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    
    for (const QHostAddress &address : addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && 
            address != QHostAddress::LocalHost) {
            return address.toString();
        }
    }
    
    return "127.0.0.1";
}