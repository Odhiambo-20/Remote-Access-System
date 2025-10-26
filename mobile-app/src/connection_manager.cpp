#include "connection_manager.h"
#include <QDebug>

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent) {
    qDebug() << "[ConnectionManager] Initialized";
}

ConnectionManager::~ConnectionManager() {
}

void ConnectionManager::setAccountServerAddress(const QString &address) {
    m_accountServerAddress = address;
    qDebug() << "[ConnectionManager] Account server address set to:" << address;
}

void ConnectionManager::login(const QString &username, const QString &password) {
    m_username = username;
    qDebug() << "[ConnectionManager] Login attempt for user:" << username;
    
    // For now, always succeed
    emit loginSuccess();
}

QString ConnectionManager::getAccountServerAddress() const {
    return m_accountServerAddress;
}
