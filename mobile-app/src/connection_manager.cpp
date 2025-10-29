
#include "connection_manager.h"
#include <QDebug>
#include <QStringList>
#include <QSettings>
#include <QCryptographicHash>

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent),
      m_isConnected(false),
      m_relaySocket(nullptr),
      m_connectionTimer(nullptr),
      m_state(Disconnected) {
    
    qDebug() << "[ConnectionManager] Initialized";
    
    // Initialize relay socket
    m_relaySocket = new QTcpSocket(this);
    connect(m_relaySocket, &QTcpSocket::connected, this, &ConnectionManager::onRelayConnected);
    connect(m_relaySocket, &QTcpSocket::disconnected, this, &ConnectionManager::onRelayDisconnected);
    connect(m_relaySocket, &QTcpSocket::readyRead, this, &ConnectionManager::onRelayReadyRead);
    connect(m_relaySocket, &QTcpSocket::errorOccurred, this, &ConnectionManager::onRelayError);
    
    // Initialize connection timeout timer
    m_connectionTimer = new QTimer(this);
    m_connectionTimer->setSingleShot(true);
    m_connectionTimer->setInterval(10000); // 10 second timeout
    connect(m_connectionTimer, &QTimer::timeout, this, &ConnectionManager::onConnectionTimeout);
    
    updateConnectionStatus("Disconnected");
}

ConnectionManager::~ConnectionManager() {
    if (m_relaySocket) {
        m_relaySocket->disconnectFromHost();
        m_relaySocket->deleteLater();
    }
}

// ============================================================================
// Account Server Operations
// ============================================================================

void ConnectionManager::setAccountServerAddress(const QString &address) {
    m_accountServerAddress = address;
    qDebug() << "[ConnectionManager] Account server address set to:" << address;
}

QString ConnectionManager::getAccountServerAddress() const {
    return m_accountServerAddress;
}

void ConnectionManager::login(const QString &username, const QString &password) {
    m_username = username;
    qDebug() << "[ConnectionManager] Login attempt for user:" << username;
    
    // TODO: Implement actual authentication with account server
    // For now, always succeed
    emit loginSuccess();
}

// ============================================================================
// QR Code Operations
// ============================================================================

void ConnectionManager::scanQRCode(const QString &qrData) {
    qDebug() << "[ConnectionManager] QR Code scanned:" << qrData;
    
    if (qrData.isEmpty()) {
        emit qrCodeError("QR code data is empty");
        return;
    }
    
    // Parse QR code
    if (parseQRCode(qrData)) {
        emit qrCodeScanned(m_currentPC.pcId, m_currentPC.username);
        qDebug() << "[ConnectionManager] QR code successfully parsed:" << m_currentPC.toString();
    }
}

bool ConnectionManager::parseQRCode(const QString &qrData) {
    qDebug() << "[ConnectionManager] Parsing QR data:" << qrData;
    
    PCConnectionInfo info = parseQRData(qrData);
    
    if (!validateQRData(info)) {
        QString error = "Invalid QR code format. Expected: pc_id|username|server|port|token";
        qDebug() << "[ConnectionManager] QR parsing failed:" << error;
        emit qrCodeParsed(false, error);
        emit qrCodeError(error);
        return false;
    }
    
    m_currentPC = info;
    saveConnectionInfo(info);
    
    QString message = QString("Connected to %1's PC").arg(info.username);
    emit qrCodeParsed(true, message);
    
    qDebug() << "[ConnectionManager] QR parsed successfully:";
    qDebug() << "   PC ID:" << info.pcId;
    qDebug() << "   Username:" << info.username;
    qDebug() << "   Relay:" << info.relayServer << ":" << info.relayPort;
    qDebug() << "   Token:" << info.authToken.left(16) << "...";
    
    return true;
}

PCConnectionInfo ConnectionManager::parseQRData(const QString &qrData) {
    PCConnectionInfo info;
    
    // Expected format: pc_id|username|relay_server|relay_port|auth_token
    QStringList parts = qrData.split('|');
    
    if (parts.size() >= 5) {
        info.pcId = parts[0].trimmed();
        info.username = parts[1].trimmed();
        info.relayServer = parts[2].trimmed();
        info.relayPort = parts[3].trimmed().toUInt();
        info.authToken = parts[4].trimmed();
    } else if (parts.size() == 4) {
        // Backward compatibility: no auth token
        info.pcId = parts[0].trimmed();
        info.username = parts[1].trimmed();
        info.relayServer = parts[2].trimmed();
        info.relayPort = parts[3].trimmed().toUInt();
        info.authToken = ""; // Will need to be generated or requested
    }
    
    return info;
}

bool ConnectionManager::validateQRData(const PCConnectionInfo &info) {
    if (info.pcId.isEmpty()) {
        qDebug() << "[ConnectionManager] Validation failed: Empty PC ID";
        return false;
    }
    
    if (info.username.isEmpty()) {
        qDebug() << "[ConnectionManager] Validation failed: Empty username";
        return false;
    }
    
    if (info.relayServer.isEmpty()) {
        qDebug() << "[ConnectionManager] Validation failed: Empty relay server";
        return false;
    }
    
    if (info.relayPort == 0 || info.relayPort > 65535) {
        qDebug() << "[ConnectionManager] Validation failed: Invalid port:" << info.relayPort;
        return false;
    }
    
    if (info.authToken.isEmpty()) {
        qDebug() << "[ConnectionManager] Warning: No auth token provided";
        // Not a fatal error, but should be noted
    }
    
    return true;
}

QString ConnectionManager::getLastScannedPCInfo() const {
    if (!m_currentPC.isValid()) {
        return "No PC scanned yet";
    }
    return m_currentPC.toString();
}

// ============================================================================
// PC Connection Operations
// ============================================================================

void ConnectionManager::connectToPC() {
    if (!m_currentPC.isValid()) {
        emit connectionError("No PC information available. Scan QR code first.");
        return;
    }
    
    connectToRelay(m_currentPC);
}

void ConnectionManager::connectToPC(const QString &pcId) {
    qDebug() << "[ConnectionManager] Connecting to saved PC:" << pcId;
    
    PCConnectionInfo info = loadConnectionInfo(pcId);
    if (!info.isValid()) {
        emit connectionError("PC not found in saved connections");
        return;
    }
    
    m_currentPC = info;
    connectToRelay(info);
}

void ConnectionManager::connectViaScan(const QString &qrData) {
    qDebug() << "[ConnectionManager] Connect via QR scan:" << qrData;
    
    if (parseQRCode(qrData)) {
        connectToRelay(m_currentPC);
    }
}

void ConnectionManager::connectToRelay(const PCConnectionInfo &info) {
    if (m_state == Connecting || m_state == Connected) {
        qDebug() << "[ConnectionManager] Already connected or connecting";
        return;
    }
    
    qDebug() << "[ConnectionManager] Connecting to relay server:" 
             << info.relayServer << ":" << info.relayPort;
    
    m_state = Connecting;
    updateConnectionStatus("Connecting to " + info.username + "'s PC...");
    
    // Disconnect if already connected
    if (m_relaySocket->state() != QAbstractSocket::UnconnectedState) {
        m_relaySocket->disconnectFromHost();
        m_relaySocket->waitForDisconnected(1000);
    }
    
    // Start connection timeout
    m_connectionTimer->start();
    
    // Connect to relay server
    m_relaySocket->connectToHost(info.relayServer, info.relayPort);
}

void ConnectionManager::disconnectFromPC() {
    qDebug() << "[ConnectionManager] Disconnecting from PC";
    
    if (m_relaySocket->state() != QAbstractSocket::UnconnectedState) {
        m_relaySocket->disconnectFromHost();
    }
    
    m_connectionTimer->stop();
    m_state = Disconnected;
    m_isConnected = false;
    m_currentPCName.clear();
    
    updateConnectionStatus("Disconnected");
    emit isConnectedChanged(false);
    emit currentPCNameChanged("");
    emit pcDisconnected();
}

// ============================================================================
// Connection State Management
// ============================================================================

QString ConnectionManager::connectionStatus() const {
    return m_connectionStatus;
}

bool ConnectionManager::isConnected() const {
    return m_isConnected;
}

QString ConnectionManager::currentPCName() const {
    return m_currentPCName;
}

void ConnectionManager::updateConnectionStatus(const QString &status) {
    if (m_connectionStatus != status) {
        m_connectionStatus = status;
        emit connectionStatusChanged(status);
        qDebug() << "[ConnectionManager] Status:" << status;
    }
}

// ============================================================================
// Socket Event Handlers
// ============================================================================

void ConnectionManager::onRelayConnected() {
    qDebug() << "[ConnectionManager] Connected to relay server";
    
    m_connectionTimer->stop();
    m_state = Authenticating;
    updateConnectionStatus("Authenticating...");
    
    sendAuthRequest(m_currentPC);
}

void ConnectionManager::sendAuthRequest(const PCConnectionInfo &info) {
    // Send authentication request to relay
    // Format: CONNECT|mobile_id|pc_id|auth_token
    QString mobileId = QCryptographicHash::hash(
        m_username.toUtf8(), 
        QCryptographicHash::Md5
    ).toHex();
    
    QString message = QString("CONNECT|%1|%2|%3\n")
        .arg(mobileId)
        .arg(info.pcId)
        .arg(info.authToken);
    
    qDebug() << "[ConnectionManager] Sending auth request:" << message.trimmed();
    m_relaySocket->write(message.toUtf8());
    m_relaySocket->flush();
}

void ConnectionManager::onRelayDisconnected() {
    qDebug() << "[ConnectionManager] Disconnected from relay server";
    
    m_connectionTimer->stop();
    
    if (m_state == Connected) {
        updateConnectionStatus("Connection lost");
        emit connectionError("Connection to PC lost");
    } else {
        updateConnectionStatus("Disconnected");
    }
    
    m_state = Disconnected;
    m_isConnected = false;
    
    emit isConnectedChanged(false);
    emit pcDisconnected();
}

void ConnectionManager::onRelayReadyRead() {
    QByteArray data = m_relaySocket->readAll();
    QString response = QString::fromUtf8(data).trimmed();
    
    qDebug() << "[ConnectionManager] Relay response:" << response;
    processRelayResponse(response);
}

void ConnectionManager::processRelayResponse(const QString &response) {
    QStringList parts = response.split('|');
    if (parts.isEmpty()) return;
    
    QString command = parts[0];
    
    if (command == "OK" && m_state == Authenticating) {
        // Authentication successful
        qDebug() << "[ConnectionManager] Authentication successful";
        
        m_state = Connected;
        m_isConnected = true;
        m_currentPCName = m_currentPC.username + "'s PC";
        
        updateConnectionStatus("Connected to " + m_currentPCName);
        emit isConnectedChanged(true);
        emit currentPCNameChanged(m_currentPCName);
        emit pcConnected(m_currentPC.pcId);
        
    } else if (command == "ERROR") {
        // Authentication or connection failed
        QString error = parts.size() > 1 ? parts[1] : "Unknown error";
        qDebug() << "[ConnectionManager] Connection error:" << error;
        
        m_state = Error;
        updateConnectionStatus("Connection failed: " + error);
        emit connectionError(error);
        
        disconnectFromPC();
        
    } else if (command == "PING") {
        // Respond to keep-alive ping
        m_relaySocket->write("PONG\n");
        m_relaySocket->flush();
        
    } else if (command == "PC_OFFLINE") {
        qDebug() << "[ConnectionManager] PC went offline";
        updateConnectionStatus("PC is offline");
        emit connectionError("PC is currently offline");
        disconnectFromPC();
    }
}

void ConnectionManager::onRelayError(QAbstractSocket::SocketError error) {
    QString errorString = m_relaySocket->errorString();
    qDebug() << "[ConnectionManager] Socket error:" << error << "-" << errorString;
    
    m_connectionTimer->stop();
    m_state = Error;
    
    updateConnectionStatus("Connection error: " + errorString);
    emit connectionError(errorString);
}

void ConnectionManager::onConnectionTimeout() {
    qDebug() << "[ConnectionManager] Connection timeout";
    
    m_state = Error;
    updateConnectionStatus("Connection timeout");
    emit connectionError("Connection timed out");
    
    if (m_relaySocket->state() != QAbstractSocket::UnconnectedState) {
        m_relaySocket->disconnectFromHost();
    }
}

// ============================================================================
// Saved Connections Management
// ============================================================================

void ConnectionManager::saveConnectionInfo(const PCConnectionInfo &info) {
    m_savedPCs[info.pcId] = info;
    
    // Save to persistent storage
    QSettings settings("RemoteAccessSystem", "MobileApp");
    settings.beginGroup("SavedPCs");
    settings.setValue(info.pcId + "/username", info.username);
    settings.setValue(info.pcId + "/relayServer", info.relayServer);
    settings.setValue(info.pcId + "/relayPort", info.relayPort);
    settings.setValue(info.pcId + "/authToken", info.authToken);
    settings.endGroup();
    
    emit pcListUpdated();
    
    qDebug() << "[ConnectionManager] Saved PC connection:" << info.pcId;
}

PCConnectionInfo ConnectionManager::loadConnectionInfo(const QString &pcId) {
    // Check memory cache first
    if (m_savedPCs.contains(pcId)) {
        return m_savedPCs[pcId];
    }
    
    // Load from persistent storage
    QSettings settings("RemoteAccessSystem", "MobileApp");
    settings.beginGroup("SavedPCs");
    
    PCConnectionInfo info;
    info.pcId = pcId;
    info.username = settings.value(pcId + "/username").toString();
    info.relayServer = settings.value(pcId + "/relayServer").toString();
    info.relayPort = settings.value(pcId + "/relayPort").toUInt();
    info.authToken = settings.value(pcId + "/authToken").toString();
    
    settings.endGroup();
    
    if (info.isValid()) {
        m_savedPCs[pcId] = info;
    }
    
    return info;
}

QStringList ConnectionManager::getSavedPCs() const {
    QSettings settings("RemoteAccessSystem", "MobileApp");
    settings.beginGroup("SavedPCs");
    QStringList pcIds = settings.childGroups();
    settings.endGroup();
    
    return pcIds;
}

void ConnectionManager::savePC(const QString &pcId, const QString &displayName) {
    if (m_savedPCs.contains(pcId)) {
        PCConnectionInfo info = m_savedPCs[pcId];
        info.username = displayName;
        saveConnectionInfo(info);
    }
}

void ConnectionManager::removePC(const QString &pcId) {
    m_savedPCs.remove(pcId);
    
    QSettings settings("RemoteAccessSystem", "MobileApp");
    settings.beginGroup("SavedPCs");
    settings.remove(pcId);
    settings.endGroup();
    
    emit pcListUpdated();
    
    qDebug() << "[ConnectionManager] Removed PC:" << pcId;
}