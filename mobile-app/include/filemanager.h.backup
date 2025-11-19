#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>

class FileManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    
public:
    explicit FileManager(QObject *parent = nullptr);
    
    QString currentPath() const { return m_currentPath; }
    void setCurrentPath(const QString &path);
    
    Q_INVOKABLE QVariantList listFiles(const QString &path);
    Q_INVOKABLE bool downloadFile(const QString &remotePath, const QString &localPath);
    Q_INVOKABLE bool uploadFile(const QString &localPath, const QString &remotePath);
    Q_INVOKABLE bool deleteFile(const QString &path);
    Q_INVOKABLE bool createDirectory(const QString &path);
    
signals:
    void currentPathChanged();
    void fileOperationCompleted(bool success, const QString &message);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    
private:
    QString m_currentPath;
};

#endif // FILEMANAGER_H
