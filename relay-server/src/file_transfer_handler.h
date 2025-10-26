#ifndef FILE_TRANSFER_HANDLER_H
#define FILE_TRANSFER_HANDLER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QList>

class RelaySession;

class FileTransferHandler : public QObject
{
    Q_OBJECT

public:
    explicit FileTransferHandler(QObject *parent = nullptr);
    ~FileTransferHandler();

    void start(quint16 port);
    void registerPCHandler(const QString &pcId, QTcpSocket *socket);
    void routeToPC(const QString &pcId, const QString &command, QTcpSocket *sourceSocket);

private slots:
    void onNewConnection();
    void onPCResponse();

private:
    void cleanup();

    QTcpServer *server;
    bool isListening;
    QMap<QString, QTcpSocket *> pcHandlers;
    QMap<QTcpSocket *, QTcpSocket *> sourceToPC;
    QMap<QTcpSocket *, QTcpSocket *> pcToSource;
    QList<RelaySession *> sessions;
};

class RelaySession : public QObject
{
    Q_OBJECT

public:
    explicit RelaySession(FileTransferHandler *handler, QObject *parent = nullptr);
    ~RelaySession();

    void setClientSocket(QTcpSocket *socket);

private slots:
    void onClientData();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    FileTransferHandler *fileHandler;
    QTcpSocket *clientSocket;
    QTcpSocket *pcSocket;
    bool isDownloading = false;
    bool isUploading = false;
    qint64 bytesRemaining = 0;
};

#endif // FILE_TRANSFER_HANDLER_H
