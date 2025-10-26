#ifndef PCMANAGER_H
#define PCMANAGER_H

#include <QObject>
#include <QString>
#include <QTcpSocket>

class PCManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString connectedPcId READ connectedPcId NOTIFY connectedPcIdChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)

public:
    explicit PCManager(QObject *parent = nullptr);
    ~PCManager();
    
    Q_INVOKABLE void queryPCList(const QString &relayServerAddress, int port = 2810);
    Q_INVOKABLE void connectToPC(const QString &pcId, const QString &relayServerAddress);
    Q_INVOKABLE void disconnectFromPC();
    
    QString connectedPcId() const { return m_connectedPcId; }
    bool isConnected() const { return m_isConnected; }

signals:
    void pcListUpdated(const QStringList &pcList);
    void connectionEstablished(const QString &pcId);
    void connectionFailed(const QString &error);
    void connectedPcIdChanged();
    void isConnectedChanged();
    void clearPCList();
    void addPCToList(const QString &pcId, const QString &hostname, const QString &username, bool isOnline);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    QTcpSocket *m_socket;
    QString m_connectedPcId;
    bool m_isConnected;
    QString m_currentPcId;
};

#endif
