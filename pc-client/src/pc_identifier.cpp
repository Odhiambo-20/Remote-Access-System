#include "pc_identifier.h"
#include <QSysInfo>
#include <QCryptographicHash>
#include <unistd.h>
#include <pwd.h>

PCIdentifier::PCIdentifier(QObject *parent) : QObject(parent) {
    // Get hostname
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    m_hostname = QString::fromUtf8(hostname);
    
    // Get username
    struct passwd *pw = getpwuid(getuid());
    m_username = QString::fromUtf8(pw->pw_name);
    
    // Generate PC ID from machine ID
    QString machineId = QSysInfo::machineUniqueId();
    if (machineId.isEmpty()) {
        machineId = m_hostname + m_username;
    }
    
    QByteArray hash = QCryptographicHash::hash(
        machineId.toUtf8(),
        QCryptographicHash::Md5
    );
    
    m_pcId = "PC-" + hash.toHex();
}

QString PCIdentifier::getPCId() {
    return m_pcId;
}

QString PCIdentifier::getUsername() {
    return m_username;
}

QString PCIdentifier::getHostname() {
    return m_hostname;
}
