#ifndef REMOTE_CONTROL_CLIENT_H
#define REMOTE_CONTROL_CLIENT_H

#include <QObject>
#include <QTcpSocket>

class RemoteControlClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit RemoteControlClient(QObject *parent = nullptr);
    
    Q_INVOKABLE void connectToPC(const QString &pcId, const QString &relayServer, int relayPort);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void sendMouseMove(int x, int y);
    Q_INVOKABLE void sendMouseClick(int button);
    Q_INVOKABLE void sendKeyPress(int key);
    
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void connectedChanged();
    void sessionStarted();
    void frameReceived(const QByteArray &frameData);
    void error(const QString &errorMessage);

private slots:
    void handleData();

private:
    QTcpSocket *m_socket;
    bool m_connected;
};

#endif // REMOTE_CONTROL_CLIENT_H
