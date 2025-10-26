#ifndef PC_IDENTIFIER_H
#define PC_IDENTIFIER_H

#include <QObject>
#include <QString>

class PCIdentifier : public QObject {
    Q_OBJECT

public:
    explicit PCIdentifier(QObject *parent = nullptr);
    QString getPCId();
    QString getUsername();
    QString getHostname();

private:
    QString m_pcId;
    QString m_username;
    QString m_hostname;
};

#endif // PC_IDENTIFIER_H
