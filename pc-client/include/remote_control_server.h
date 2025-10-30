#ifndef REMOTE_CONTROL_SERVER_H
#define REMOTE_CONTROL_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class RemoteControlServer : public QObject {
    Q_OBJECT

public:
    explicit RemoteControlServer(QObject *parent = nullptr);
    bool start(int port = 2812);

private slots:
    void handleNewConnection();
    void handleClientCommand(QTcpSocket *client);

private:
    QTcpServer *m_server;
};

#endif // REMOTE_CONTROL_SERVER_H