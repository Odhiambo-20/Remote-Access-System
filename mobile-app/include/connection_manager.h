#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
#include <QObject>
#include <QString>
#include <QTcpSocket>
class ConnectionManager : public QObject {
    Q_OBJECT
public:
    explicit ConnectionManager(QObject *parent = nullptr);
    ~ConnectionManager();
    Q_INVOKABLE bool connectToServer(const QString &host, int port);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE bool isConnected() const;
signals:
    void connectedChanged();
    void errorOccurred(const QString &error);
private:
    QTcpSocket *socket;
    bool connected;
};
#endif
