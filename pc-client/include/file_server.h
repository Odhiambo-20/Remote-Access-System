#ifndef FILE_SERVER_H
#define FILE_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QDateTime>

struct ShareInfo {
    QString filePath;
    QDateTime expiryTime;
};

class FileServer : public QObject {
    Q_OBJECT

public:
    explicit FileServer(QObject *parent = nullptr);
    bool start(int port = 2811, int httpPort = 8080);

private slots:
    void handleNewConnection();
    void handleHttpConnection();
    void handleClientData(QTcpSocket *client);

private:
    void listDirectory(QTcpSocket *client, const QString &path);
    void downloadFile(QTcpSocket *client, const QString &path);
    void uploadFile(QTcpSocket *client, const QString &path, qint64 size);
    void deleteFile(QTcpSocket *client, const QString &path);
    void generateShareLink(QTcpSocket *client, const QString &path, int expiryHours);
    void serveSharedFile(QTcpSocket *client, const QString &token);
    QString getLocalIP();

    QTcpServer *m_server;
    QTcpServer *m_httpServer;
    QMap<QString, ShareInfo> m_shareLinks;
};

#endif // FILE_SERVER_H
