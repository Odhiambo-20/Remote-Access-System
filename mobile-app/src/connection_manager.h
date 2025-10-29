#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QMap>
#include <QTimer>

/**
 * Struct to hold PC connection information parsed from QR code
 */
struct PCConnectionInfo {
    QString pcId;           // Unique PC identifier
    QString username;       // PC owner username
    QString relayServer;    // Relay server address
    uint16_t relayPort;     // Relay server port
    QString authToken;      // Authentication token
    
    bool isValid() const {
        return !pcId.isEmpty() && !username.isEmpty() && 
               !relayServer.isEmpty() && relayPort > 0 && !authToken.isEmpty();
    }
    
    QString toString() const {
        return QString("PC[%1] User[%2] @ %3:%4 Token[%5]")
            .arg(pcId, username, relayServer)
            .arg(relayPort)
            .arg(authToken.left(8) + "...");
    }
};

/**
 * ConnectionManager handles all network connections including:
 * - QR code scanning and parsing
 * - Account server authentication
 * - PC connection via relay server
 * - Connection state management
 */
class ConnectionManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString currentPCName READ currentPCName NOTIFY currentPCNameChanged)

public:
    explicit ConnectionManager(QObject *parent = nullptr);
    ~ConnectionManager();

    // Account Server Operations
    Q_INVOKABLE void setAccountServerAddress(const QString &address);
    Q_INVOKABLE QString getAccountServerAddress() const;
    Q_INVOKABLE void login(const QString &username, const QString &password);
    
    // QR Code Operations
    Q_INVOKABLE void scanQRCode(const QString &qrData);
    Q_INVOKABLE bool parseQRCode(const QString &qrData);
    Q_INVOKABLE QString getLastScannedPCInfo() const;
    
    // PC Connection Operations
    Q_INVOKABLE void connectToPC();
    Q_INVOKABLE void connectToPC(const QString &pcId);
    Q_INVOKABLE void disconnectFromPC();
    Q_INVOKABLE void connectViaScan(const QString &qrData);
    
    // Connection State
    Q_INVOKABLE QString connectionStatus() const;
    Q_INVOKABLE bool isConnected() const;
    Q_INVOKABLE QString currentPCName() const;
    
    // Stored Connections
    Q_INVOKABLE QStringList getSavedPCs() const;
    Q_INVOKABLE void savePC(const QString &pcId, const QString &displayName);
    Q_INVOKABLE void removePC(const QString &pcId);

signals:
    // Login signals
    void loginSuccess();
    void loginFailed(const QString &error);
    
    // QR Code signals
    void qrCodeScanned(const QString &pcId, const QString &username);
    void qrCodeParsed(bool success, const QString &message);
    void qrCodeError(const QString &error);
    
    // Connection signals
    void connectionStatusChanged(const QString &status);
    void isConnectedChanged(bool connected);
    void currentPCNameChanged(const QString &name);
    void pcConnected(const QString &pcId);
    void pcDisconnected();
    void connectionError(const QString &error);
    
    // Data signals
    void pcListUpdated();

private slots:
    void onRelayConnected();
    void onRelayDisconnected();
    void onRelayReadyRead();
    void onRelayError(QAbstractSocket::SocketError error);
    void onConnectionTimeout();

private:
    // Helper methods
    PCConnectionInfo parseQRData(const QString &qrData);
    bool validateQRData(const PCConnectionInfo &info);
    void connectToRelay(const PCConnectionInfo &info);
    void sendAuthRequest(const PCConnectionInfo &info);
    void processRelayResponse(const QString &response);
    void updateConnectionStatus(const QString &status);
    void saveConnectionInfo(const PCConnectionInfo &info);
    PCConnectionInfo loadConnectionInfo(const QString &pcId);
    
    // Member variables
    QString m_accountServerAddress;
    QString m_username;
    QString m_connectionStatus;
    QString m_currentPCName;
    bool m_isConnected;
    
    // Current connection
    QTcpSocket *m_relaySocket;
    PCConnectionInfo m_currentPC;
    QTimer *m_connectionTimer;
    
    // Saved connections
    QMap<QString, PCConnectionInfo> m_savedPCs;
    
    // Connection state
    enum ConnectionState {
        Disconnected,
        Connecting,
        Authenticating,
        Connected,
        Error
    };
    ConnectionState m_state;
};

#endif // CONNECTION_MANAGER_H