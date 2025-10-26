#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <QObject>
#include <QString>

class ConnectionManager : public QObject {
    Q_OBJECT

public:
    explicit ConnectionManager(QObject *parent = nullptr);
    ~ConnectionManager();

    Q_INVOKABLE void setAccountServerAddress(const QString &address);
    Q_INVOKABLE void login(const QString &username, const QString &password);
    Q_INVOKABLE QString getAccountServerAddress() const;

signals:
    void loginSuccess();
    void loginFailed(const QString &error);

private:
    QString m_accountServerAddress;
    QString m_username;
};

#endif
