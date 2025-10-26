#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

class FileManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(bool isUploading READ isUploading NOTIFY isUploadingChanged)
    Q_PROPERTY(int uploadProgress READ uploadProgress NOTIFY uploadProgressChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)

public:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();
    
    bool isBusy() const { return m_isBusy; }
    bool isUploading() const { return m_isUploading; }
    int uploadProgress() const { return m_uploadProgress; }
    QString currentPath() const { return m_currentPath; }
    
    Q_INVOKABLE void connectToPC(const QString &pcId, const QString &relayServer);
    Q_INVOKABLE void browseDirectory(const QString &path);
    Q_INVOKABLE void listDirectory(const QString &path);
    Q_INVOKABLE void uploadFile(const QString &localPath, const QString &remotePath);
    Q_INVOKABLE void downloadFile(const QString &remotePath, const QString &localPath);
    Q_INVOKABLE void deleteFile(const QString &remotePath);
    Q_INVOKABLE void generateShareLink(const QString &remotePath, int expiryHours);
    Q_INVOKABLE void generateShareUrl(const QString &remotePath);
    Q_INVOKABLE void createShareLink(const QString &filePath);
    Q_INVOKABLE void openFile(const QString &filePath);
    Q_INVOKABLE void goBack();
    Q_INVOKABLE void goHome();
    Q_INVOKABLE void disconnect();

signals:
    void connected();
    void disconnected();
    void connectionFailed(const QString &error);
    
    void directoryListed(const QStringList &entries);
    void directoryListReceived(const QStringList &files);
    void clearFileList();
    void addFileToList(const QString &name, const QString &path, const QString &type, qint64 size, bool isDir);
    
    void uploadStarted();
    void uploadFinished(bool success, const QString &message);
    void uploadProgress(int percent);
    void uploadProgressChanged();
    void isUploadingChanged();
    
    void downloadProgress(int percent);
    void downloadCompleted(const QString &localPath);
    
    void shareLinkGenerated(const QString &url);
    void shareLinkCreated(const QString &url);
    void shareUrlGenerated(const QString &url);
    
    void fileOpened(const QString &filePath);
    void completed();
    void error(const QString &message);
    void errorOccurred(const QString &message);
    void busyChanged();
    void currentPathChanged();

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    void setBusy(bool busy);
    void sendCommand(const QString &command);
    void handleDirectoryListing(const QString &response);
    void handleResponse(const QString &response);
    void processFileUpload();
    void processFileUploadToSocket(QTcpSocket *socket);
    
    QTcpSocket *m_socket;
    QTcpSocket *m_fileSocket;
    QString m_currentPCId;
    QString m_pcId;
    QString m_relayServerAddress;
    QString m_relayHost;
    int m_relayPort;
    QString m_currentPath;
    QStringList m_pathHistory;
    
    // Upload-related members
    bool m_isUploading;
    int m_uploadProgress;
    QFile *m_uploadFile;
    qint64 m_uploadTotalBytes;
    qint64 m_uploadSentBytes;
    QString m_uploadRemotePath;
    
    // Download-related members
    QFile m_downloadFile;
    QString m_downloadPath;
    qint64 m_expectedFileSize;
    qint64 m_receivedBytes;
    
    bool m_isBusy;
};

#endif // FILE_MANAGER_H
