#include "filemanager.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

FileManager::FileManager(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_fileSocket(nullptr)
    , m_isUploading(false)
    , m_uploadProgress(0)
    , m_uploadFile(nullptr)
    , m_uploadTotalBytes(0)
    , m_uploadSentBytes(0)
    , m_currentPath("/")
    , m_isBusy(false)
    , m_expectedFileSize(0)
    , m_receivedBytes(0)
{
    qDebug() << "[FileManager] Initialized";
    
    connect(m_socket, &QTcpSocket::connected, this, &FileManager::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &FileManager::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &FileManager::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &FileManager::onError);
}

FileManager::~FileManager() {
    if (m_uploadFile) {
        m_uploadFile->close();
        delete m_uploadFile;
    }
    if (m_fileSocket) {
        m_fileSocket->close();
        m_fileSocket->deleteLater();
    }
    if (m_socket->isOpen()) {
        m_socket->close();
    }
}

void FileManager::setBusy(bool busy) {
    if (m_isBusy != busy) {
        m_isBusy = busy;
        emit busyChanged();
    }
}

void FileManager::connectToPC(const QString &pcId, const QString &relayServerAddress) {
    qDebug() << "[FileManager] Connecting to PC:" << pcId << "via" << relayServerAddress;
    
    m_pcId = pcId;
    m_relayServerAddress = relayServerAddress;
    
    // Parse relay server address
    QStringList parts = relayServerAddress.split(':');
    QString host = parts.value(0, "127.0.0.1");
    int port = parts.value(1, "2810").toInt();
    
    qDebug() << "[FileManager] Connecting to relay server:" << host << ":" << port;
    
    m_relayHost = host;
    m_relayPort = port;
    
    // Signal that we're ready
    emit connected();
}

void FileManager::browseDirectory(const QString &path) {
    qDebug() << "[FileManager] Browsing directory:" << path;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        return;
    }
    
    setBusy(true);
    m_currentPath = path;
    emit currentPathChanged();
    
    // Create new socket for this request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, path]() {
        qDebug() << "[FileManager] Connected to relay for LIST_DIR request";
        
        // Send LIST_DIR command to relay
        QString command = QString("LIST_DIR|%1|%2\n").arg(m_pcId, path);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Received:" << response;
        
        handleDirectoryListing(response);
        setBusy(false);
        
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to browse directory: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::listDirectory(const QString &path) {
    qDebug() << "[FileManager] Listing directory:" << path;
    browseDirectory(path);
}

void FileManager::openFile(const QString &filePath) {
    qDebug() << "[FileManager] Opening file:" << filePath;
    
    QString command = QString("FILE_OPEN|%1|%2").arg(m_pcId, filePath);
    sendCommand(command);
}

void FileManager::uploadFile(const QString &localFilePath, const QString &remotePath) {
    qDebug() << "[FileManager] Uploading file:" << localFilePath << "to" << remotePath;
    
    if (m_isUploading) {
        qDebug() << "[FileManager] Already uploading a file";
        emit errorOccurred("Upload already in progress");
        return;
    }
    
    // Remove file:// prefix if present
    QString cleanPath = localFilePath;
    if (cleanPath.startsWith("file://")) {
        cleanPath = cleanPath.mid(7);
    }
    
    // Open the file to upload
    m_uploadFile = new QFile(cleanPath);
    if (!m_uploadFile->open(QIODevice::ReadOnly)) {
        qDebug() << "[FileManager] Failed to open file:" << m_uploadFile->errorString();
        emit errorOccurred("Failed to open file: " + m_uploadFile->errorString());
        delete m_uploadFile;
        m_uploadFile = nullptr;
        return;
    }
    
    m_uploadTotalBytes = m_uploadFile->size();
    m_uploadSentBytes = 0;
    m_uploadRemotePath = remotePath;
    m_uploadProgress = 0;
    m_isUploading = true;
    
    emit isUploadingChanged();
    emit uploadStarted();
    emit uploadProgressChanged();
    
    // Get filename
    QFileInfo fileInfo(cleanPath);
    QString fileName = fileInfo.fileName();
    
    qDebug() << "[FileManager] File size:" << m_uploadTotalBytes << "bytes, name:" << fileName;
    
    // Create dedicated socket for upload
    if (m_fileSocket) {
        m_fileSocket->close();
        m_fileSocket->deleteLater();
    }
    
    m_fileSocket = new QTcpSocket(this);
    
    connect(m_fileSocket, &QTcpSocket::connected, this, [this, remotePath, fileName]() {
        qDebug() << "[FileManager] Upload socket connected, sending UPLOAD command";
        
        // Send UPLOAD command with correct format: UPLOAD|pc_id|remote_path/filename|file_size
        QString fullPath = remotePath;
        if (!fullPath.endsWith('/')) {
            fullPath += '/';
        }
        fullPath += fileName;
        
        QString command = QString("UPLOAD|%1|%2|%3\n")
                              .arg(m_pcId)
                              .arg(fullPath)
                              .arg(m_uploadTotalBytes);
        
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        m_fileSocket->write(command.toUtf8());
        m_fileSocket->flush();
    });
    
    connect(m_fileSocket, &QTcpSocket::readyRead, this, [this]() {
        QByteArray data = m_fileSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Upload response:" << response;
        
        if (response.startsWith("UPLOAD_READY")) {
            qDebug() << "[FileManager] Server ready, starting file transfer";
            processFileUploadToSocket(m_fileSocket);
        }
        else if (response.startsWith("UPLOAD_COMPLETE") || response.startsWith("UPLOAD_SUCCESS")) {
            qDebug() << "[FileManager] Upload completed successfully";
            
            if (m_uploadFile) {
                m_uploadFile->close();
                delete m_uploadFile;
                m_uploadFile = nullptr;
            }
            
            m_isUploading = false;
            m_uploadProgress = 100;
            emit uploadProgressChanged();
            emit isUploadingChanged();
            emit uploadFinished(true, "File uploaded successfully");
            
            m_fileSocket->close();
        }
        else if (response.startsWith("ERROR")) {
            QString error = response.mid(6);
            qDebug() << "[FileManager] Upload failed:" << error;
            
            if (m_uploadFile) {
                m_uploadFile->close();
                delete m_uploadFile;
                m_uploadFile = nullptr;
            }
            
            m_isUploading = false;
            emit isUploadingChanged();
            emit uploadFinished(false, error);
            
            m_fileSocket->close();
        }
    });
    
    connect(m_fileSocket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Upload socket error:" << m_fileSocket->errorString();
        
        if (m_uploadFile) {
            m_uploadFile->close();
            delete m_uploadFile;
            m_uploadFile = nullptr;
        }
        
        m_isUploading = false;
        emit isUploadingChanged();
        emit uploadFinished(false, "Connection error: " + m_fileSocket->errorString());
    });
    
    connect(m_fileSocket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "[FileManager] Upload socket disconnected";
        
        if (m_isUploading) {
            // Unexpected disconnect
            if (m_uploadFile) {
                m_uploadFile->close();
                delete m_uploadFile;
                m_uploadFile = nullptr;
            }
            
            m_isUploading = false;
            emit isUploadingChanged();
            emit uploadFinished(false, "Connection lost during upload");
        }
    });
    
    // Connect to relay server
    qDebug() << "[FileManager] Connecting upload socket to" << m_relayHost << ":" << m_relayPort;
    m_fileSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::downloadFile(const QString &remotePath, const QString &localPath) {
    qDebug() << "[FileManager] Downloading file from:" << remotePath << "to" << localPath;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        emit errorOccurred("Another operation is in progress");
        return;
    }
    
    setBusy(true);
    
    // If no local path specified, use Downloads folder
    QString cleanPath = localPath;
    if (cleanPath.isEmpty()) {
        QFileInfo fileInfo(remotePath);
        QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        cleanPath = downloadPath + "/" + fileInfo.fileName();
    }
    
    // Remove file:// prefix if present
    if (cleanPath.startsWith("file://")) {
        cleanPath = cleanPath.mid(7);
    }
    
    m_downloadPath = cleanPath;
    m_receivedBytes = 0;
    
    // Create new socket for download request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, remotePath]() {
        qDebug() << "[FileManager] Connected to relay for DOWNLOAD request";
        
        QString command = QString("DOWNLOAD|%1|%2\n").arg(m_pcId, remotePath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        
        if (m_receivedBytes == 0) {
            // First packet should contain file size
            QString response = QString::fromUtf8(data).trimmed();
            qDebug() << "[FileManager] Download response:" << response;
            
            if (response.startsWith("DOWNLOAD_READY|")) {
                QStringList parts = response.split('|');
                if (parts.size() >= 2) {
                    m_expectedFileSize = parts[1].toLongLong();
                    qDebug() << "[FileManager] Expected file size:" << m_expectedFileSize;
                    
                    // Open file for writing
                    m_downloadFile.setFileName(m_downloadPath);
                    if (!m_downloadFile.open(QIODevice::WriteOnly)) {
                        qDebug() << "[FileManager] Failed to open download file:" << m_downloadFile.errorString();
                        emit errorOccurred("Failed to create download file: " + m_downloadFile.errorString());
                        setBusy(false);
                        requestSocket->disconnectFromHost();
                        requestSocket->deleteLater();
                        return;
                    }
                }
            } else if (response.startsWith("ERROR")) {
                QString error = response.mid(6);
                emit errorOccurred("Download failed: " + error);
                setBusy(false);
                requestSocket->disconnectFromHost();
                requestSocket->deleteLater();
                return;
            }
        } else {
            // Subsequent packets contain file data
            m_downloadFile.write(data);
            m_receivedBytes += data.size();
            
            int progress = (m_receivedBytes * 100) / m_expectedFileSize;
            emit downloadProgress(progress);
            
            qDebug() << "[FileManager] Download progress:" << progress << "%";
            
            if (m_receivedBytes >= m_expectedFileSize) {
                m_downloadFile.close();
                qDebug() << "[FileManager] Download completed";
                emit downloadCompleted(m_downloadPath);
                emit errorOccurred("Download completed: " + m_downloadPath);
                setBusy(false);
                requestSocket->disconnectFromHost();
                requestSocket->deleteLater();
            }
        }
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Download socket error:" << requestSocket->errorString();
        emit errorOccurred("Download failed: " + requestSocket->errorString());
        if (m_downloadFile.isOpen()) {
            m_downloadFile.close();
        }
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::downloadMultipleFiles(const QVariantList &files) {
    qDebug() << "[FileManager] Downloading" << files.size() << "files";
    
    for (const QVariant &fileVar : files) {
        QVariantMap fileMap = fileVar.toMap();
        QString filePath = fileMap["path"].toString();
        
        if (!filePath.isEmpty()) {
            downloadFile(filePath, "");
        }
    }
}

void FileManager::copyFile(const QString &sourcePath, const QString &destinationPath) {
    qDebug() << "[FileManager] Copying file from" << sourcePath << "to" << destinationPath;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        emit errorOccurred("Another operation is in progress");
        return;
    }
    
    setBusy(true);
    
    // Create new socket for copy request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, sourcePath, destinationPath]() {
        qDebug() << "[FileManager] Connected to relay for COPY request";
        
        QString command = QString("COPY|%1|%2|%3\n").arg(m_pcId, sourcePath, destinationPath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Copy response:" << response;
        
        if (response.startsWith("COPY_SUCCESS")) {
            emit completed();
            emit errorOccurred("File copied successfully");
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to copy file: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to copy file: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::moveFile(const QString &sourcePath, const QString &destinationPath) {
    qDebug() << "[FileManager] Moving file from" << sourcePath << "to" << destinationPath;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        emit errorOccurred("Another operation is in progress");
        return;
    }
    
    setBusy(true);
    
    // Create new socket for move request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, sourcePath, destinationPath]() {
        qDebug() << "[FileManager] Connected to relay for MOVE request";
        
        QString command = QString("MOVE|%1|%2|%3\n").arg(m_pcId, sourcePath, destinationPath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Move response:" << response;
        
        if (response.startsWith("MOVE_SUCCESS")) {
            emit completed();
            emit errorOccurred("File moved successfully");
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to move file: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to move file: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::renameFile(const QString &oldPath, const QString &newName) {
    qDebug() << "[FileManager] Renaming file" << oldPath << "to" << newName;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        emit errorOccurred("Another operation is in progress");
        return;
    }
    
    setBusy(true);
    
    // Extract directory path
    QString dirPath = oldPath.left(oldPath.lastIndexOf('/'));
    QString newPath = dirPath + "/" + newName;
    
    // Create new socket for rename request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, oldPath, newPath]() {
        qDebug() << "[FileManager] Connected to relay for RENAME request";
        
        QString command = QString("RENAME|%1|%2|%3\n").arg(m_pcId, oldPath, newPath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Rename response:" << response;
        
        if (response.startsWith("RENAME_SUCCESS")) {
            emit completed();
            emit errorOccurred("File renamed successfully");
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to rename file: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to rename file: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::createFolder(const QString &folderPath) {
    qDebug() << "[FileManager] Creating folder:" << folderPath;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        emit errorOccurred("Another operation is in progress");
        return;
    }
    
    setBusy(true);
    
    // Create new socket for create folder request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, folderPath]() {
        qDebug() << "[FileManager] Connected to relay for CREATE_FOLDER request";
        
        QString command = QString("CREATE_FOLDER|%1|%2\n").arg(m_pcId, folderPath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Create folder response:" << response;
        
        if (response.startsWith("CREATE_FOLDER_SUCCESS")) {
            emit completed();
            emit errorOccurred("Folder created successfully");
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to create folder: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to create folder: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::deleteFile(const QString &remotePath) {
    qDebug() << "[FileManager] Deleting file:" << remotePath;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        return;
    }
    
    setBusy(true);
    
    // Create new socket for delete request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, remotePath]() {
        qDebug() << "[FileManager] Connected to relay for DELETE request";
        
        QString command = QString("DELETE|%1|%2\n").arg(m_pcId, remotePath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Delete response:" << response;
        
        if (response.startsWith("DELETE_SUCCESS")) {
            emit completed();
            emit errorOccurred("File deleted successfully");
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to delete file: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to delete file: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::generateShareLink(const QString &remotePath, int expiryHours) {
    qDebug() << "[FileManager] Generating share link for:" << remotePath << "expiry:" << expiryHours << "hours";
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        return;
    }
    
    setBusy(true);
    
    // Create new socket for share link request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, remotePath, expiryHours]() {
        qDebug() << "[FileManager] Connected to relay for GENERATE_LINK request";
        
        QString command = QString("GENERATE_LINK|%1|%2|%3\n").arg(m_pcId, remotePath).arg(expiryHours);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Share link response:" << response;
        
        if (response.startsWith("SHARE_LINK|")) {
            QString url = response.mid(11);
            emit shareLinkGenerated(url);
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to generate share link: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to generate share link: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::generateShareUrl(const QString &remotePath) {
    qDebug() << "[FileManager] Generating share URL for:" << remotePath;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        return;
    }
    
    setBusy(true);
    
    // Create new socket for share URL request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, remotePath]() {
        qDebug() << "[FileManager] Connected to relay for GENERATE_URL request";
        
        QString command = QString("GENERATE_URL|%1|%2\n").arg(m_pcId, remotePath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Share URL response:" << response;
        
        if (response.startsWith("SHARE_URL|")) {
            QString url = response.mid(10);
            emit shareUrlGenerated(url);
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to generate share URL: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to generate share URL: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::createShareLink(const QString &filePath) {
    qDebug() << "[FileManager] Creating share link for:" << filePath;
    
    if (m_isBusy) {
        qDebug() << "[FileManager] Busy, ignoring request";
        return;
    }
    
    setBusy(true);
    
    // Create new socket for share link request
    QTcpSocket *requestSocket = new QTcpSocket(this);
    
    connect(requestSocket, &QTcpSocket::connected, this, [this, requestSocket, filePath]() {
        qDebug() << "[FileManager] Connected to relay for GENERATE_URL request";
        
        QString command = QString("GENERATE_URL|%1|%2\n").arg(m_pcId, filePath);
        qDebug() << "[FileManager] Sending:" << command.trimmed();
        requestSocket->write(command.toUtf8());
        requestSocket->flush();
    });
    
    connect(requestSocket, &QTcpSocket::readyRead, this, [this, requestSocket]() {
        QByteArray data = requestSocket->readAll();
        QString response = QString::fromUtf8(data).trimmed();
        qDebug() << "[FileManager] Share link response:" << response;
        
        if (response.startsWith("SHARE_URL|")) {
            QString url = response.mid(10);
            emit shareLinkCreated(url);
        } else if (response.startsWith("ERROR|")) {
            QString error = response.mid(6);
            emit errorOccurred("Failed to create share link: " + error);
        }
        
        setBusy(false);
        requestSocket->disconnectFromHost();
        requestSocket->deleteLater();
    });
    
    connect(requestSocket, &QTcpSocket::errorOccurred, this, [this, requestSocket](QAbstractSocket::SocketError error) {
        qDebug() << "[FileManager] Socket error:" << requestSocket->errorString();
        emit errorOccurred("Failed to create share link: " + requestSocket->errorString());
        setBusy(false);
        requestSocket->deleteLater();
    });
    
    requestSocket->connectToHost(m_relayHost, m_relayPort);
}

void FileManager::disconnect() {
    qDebug() << "[FileManager] Disconnecting from PC";
    
    if (m_socket->isOpen()) {
        m_socket->disconnectFromHost();
    }
    
    if (m_fileSocket && m_fileSocket->isOpen()) {
        m_fileSocket->disconnectFromHost();
    }
    
    m_pcId.clear();
    m_relayServerAddress.clear();
    m_currentPath = "/";
    m_pathHistory.clear();
    
    emit disconnected();
}

void FileManager::goBack() {
    if (m_pathHistory.isEmpty()) {
        return;
    }
    
    QString previousPath = m_pathHistory.takeLast();
    browseDirectory(previousPath);
}

void FileManager::goHome() {
    m_pathHistory.clear();
    browseDirectory("/home");
}

void FileManager::onConnected() {
    qDebug() << "[FileManager] Connected to relay server";
    emit connected();
}

void FileManager::onReadyRead() {
    QByteArray data = m_socket->readAll();
    QString response = QString::fromUtf8(data).trimmed();
    
    qDebug() << "[FileManager] Response:" << response;
    
    handleResponse(response);
}

void FileManager::onDisconnected() {
    qDebug() << "[FileManager] Disconnected from relay server";
    emit disconnected();
}

void FileManager::onError(QAbstractSocket::SocketError error) {
    qDebug() << "[FileManager] Socket error:" << error << m_socket->errorString();
    emit connectionFailed(m_socket->errorString());
}

void FileManager::sendCommand(const QString &command) {
    if (!m_socket->isOpen()) {
        qDebug() << "[FileManager] Socket not open, cannot send command";
        return;
    }
    
    qDebug() << "[FileManager] Sending:" << command;
    m_socket->write(command.toUtf8() + "\n");
    m_socket->flush();
}

void FileManager::handleDirectoryListing(const QString &response) {
    if (response.startsWith("DIR_LIST|")) {
        // Clear current file list
        emit clearFileList();
        
        // Parse response: DIR_LIST|name1|type1|size1;name2|type2|size2;...
        QString content = response.mid(9);  // Skip "DIR_LIST|"
        
        QStringList entries = content.split(';', Qt::SkipEmptyParts);
        qDebug() << "[FileManager] Processing" << entries.size() << "file entries";
        
        for (const QString &entry : entries) {
            QStringList parts = entry.split('|');
            if (parts.size() >= 3) {
                QString name = parts[0];
                QString type = parts[1];
                qint64 size = parts[2].toLongLong();
                bool isDir = (type == "dir");
                
                QString fullPath = m_currentPath;
                if (!fullPath.endsWith('/')) {
                    fullPath += '/';
                }
                fullPath += name;
                
                qDebug() << "[FileManager] Adding file:" << name << "type:" << type << "size:" << size;
                emit addFileToList(name, fullPath, type, size, isDir);
            }
        }
        
        emit directoryListed(QStringList());
    }
    else if (response.startsWith("ERROR|")) {
        QString error = response.mid(6);
        qDebug() << "[FileManager] Error from server:" << error;
        emit errorOccurred(error);
    }
}

void FileManager::handleResponse(const QString &response) {
    QStringList parts = response.split('|');
    
    if (parts.isEmpty()) {
        return;
    }
    
    QString command = parts[0];
    
    if (command == "FILE_LIST") {
        emit clearFileList();
        
        if (parts.size() > 1) {
            QString fileData = parts[1];
            QStringList entries = fileData.split(';', Qt::SkipEmptyParts);
            
            for (const QString &entry : entries) {
                QStringList fields = entry.split(',');
                if (fields.size() >= 4) {
                    QString name = fields[0];
                    QString type = fields[1];
                    qint64 size = fields[2].toLongLong();
                    bool isDir = fields[3].toInt() == 1;
                    
                    QString fullPath = m_currentPath;
                    if (!fullPath.endsWith('/')) {
                        fullPath += '/';
                    }
                    fullPath += name;
                    
                    emit addFileToList(name, fullPath, type, size, isDir);
                }
            }
        }
        
        emit directoryListed(QStringList());
    }
    else if (command == "FILE_OPENED") {
        if (parts.size() > 1) {
            emit fileOpened(parts[1]);
        }
    }
    else if (command == "SHARE_LINK") {
        if (parts.size() > 1) {
            QString link = parts[1];
            qDebug() << "[FileManager] Share link created:" << link;
            emit shareLinkCreated(link);
        }
    }
    else if (command == "ERROR") {
        QString error = parts.value(1, "Unknown error");
        qDebug() << "[FileManager] Error:" << error;
        emit errorOccurred(error);
    }
}

void FileManager::processFileUploadToSocket(QTcpSocket *socket) {
    if (!m_uploadFile || !m_uploadFile->isOpen() || !socket || !socket->isOpen()) {
        qDebug() << "[FileManager] Cannot upload - invalid state";
        return;
    }
    
    qDebug() << "[FileManager] Starting file data transfer";
    
    const qint64 chunkSize = 8192; // 8KB chunks
    
    while (m_uploadSentBytes < m_uploadTotalBytes) {
        QByteArray chunk = m_uploadFile->read(chunkSize);
        if (chunk.isEmpty()) {
            break;
        }
        
        qint64 written = socket->write(chunk);
        if (written == -1) {
            qDebug() << "[FileManager] Write error:" << socket->errorString();
            break;
        }
        
        m_uploadSentBytes += written;
        
        int progress = (m_uploadSentBytes * 100) / m_uploadTotalBytes;
        if (progress != m_uploadProgress) {
            m_uploadProgress = progress;
            emit uploadProgressChanged();
            qDebug() << "[FileManager] Upload progress:" << progress << "%";
        }
        
        // Flush every 100KB
        if (m_uploadSentBytes % 102400 == 0) {
            socket->flush();
        }
    }
    
    socket->flush();
    
    qDebug() << "[FileManager] File data sent:" << m_uploadSentBytes << "/" << m_uploadTotalBytes << "bytes";
}

void FileManager::processFileUpload() {
    // This is kept for backward compatibility but not used with new implementation
    processFileUploadToSocket(m_socket);
}