#include "filemanager.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>

FileManager::FileManager(QObject *parent)
    : QObject(parent)
{
}

void FileManager::setCurrentPath(const QString &path)
{
    if (m_currentPath != path) {
        m_currentPath = path;
        emit currentPathChanged();
    }
}

QVariantList FileManager::listFiles(const QString &path)
{
    QVariantList files;
    QDir dir(path.isEmpty() ? m_currentPath : path);
    
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : fileList) {
        QVariantMap fileMap;
        fileMap["name"] = fileInfo.fileName();
        fileMap["path"] = fileInfo.absoluteFilePath();
        fileMap["isDir"] = fileInfo.isDir();
        fileMap["size"] = fileInfo.size();
        fileMap["modified"] = fileInfo.lastModified();
        files.append(fileMap);
    }
    
    return files;
}

void FileManager::downloadFile(const QString &remotePath, const QString &localPath)
{
    // Placeholder implementation
    emit fileOperationCompleted(true, "Download started");
}

void FileManager::uploadFile(const QString &localPath, const QString &remotePath)
{
    // Placeholder implementation
    emit fileOperationCompleted(true, "Upload started");
}

void FileManager::deleteFile(const QString &remotePath)
{
    QFile file(remotePath);
    bool success = file.remove();
    emit fileOperationCompleted(success, success ? "File deleted" : "Failed to delete file");
}

bool FileManager::createDirectory(const QString &path)
{
    QDir dir;
    bool success = dir.mkpath(path);
    emit fileOperationCompleted(success, success ? "Directory created" : "Failed to create directory");
    return success;
}
