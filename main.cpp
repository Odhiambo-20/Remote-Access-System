#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "include/connection_manager.h"
#include "include/pc_manager.h"
#include "include/file_manager.h"
#include "include/settings_manager.h"

int main(int argc, char *argv[])
{
    qDebug() << "===========================================";
    qDebug() << "Remote Access Mobile App - PRODUCTION MODE";
    qDebug() << "===========================================";
    qDebug() << "";
    
    QGuiApplication app(argc, argv);
    
    // Set application metadata
    app.setOrganizationName("RemoteAccessSystem");
    app.setOrganizationDomain("remote-access.local");
    app.setApplicationName("RemoteAccessMobile");
    app.setApplicationVersion("1.0.0");

    qDebug() << "[Main] Creating managers...";
    
    // Create manager instances with proper parent relationships
    auto* settingsManager = new RemoteAccessSystem::Mobile::SettingsManager(&app);
    auto* connectionManager = new RemoteAccessSystem::Mobile::ConnectionManager(&app);
    auto* pcManager = new RemoteAccessSystem::Mobile::PCManager(connectionManager);
    auto* fileManager = new RemoteAccessSystem::Mobile::FileManager(connectionManager);
    
    qDebug() << "[Main] ✓ All managers created";
    
    // Load settings
    if (settingsManager->loadSettings()) {
        qDebug() << "[Main] ✓ Settings loaded";
        qDebug() << "[Main]   Account Server:" << settingsManager->getAccountServerAddress();
        qDebug() << "[Main]   Relay Server:" << settingsManager->getRelayServerAddress();
    } else {
        qDebug() << "[Main] ! Using default settings";
    }
    
    // Auto-connect to servers on startup
    QString accountServer = settingsManager->getAccountServerAddress();
    QString relayServer = settingsManager->getRelayServerAddress();
    
    if (!accountServer.isEmpty()) {
        qDebug() << "[Main] Auto-connecting to Account Server...";
        if (connectionManager->connectToAccountServer(accountServer)) {
            qDebug() << "[Main] ✓ Connected to Account Server";
        } else {
            qDebug() << "[Main] ✗ Failed to connect to Account Server";
        }
    }
    
    if (!relayServer.isEmpty()) {
        qDebug() << "[Main] Auto-connecting to Relay Server...";
        if (connectionManager->connectToRelayServer(relayServer)) {
            qDebug() << "[Main] ✓ Connected to Relay Server";
        } else {
            qDebug() << "[Main] ✗ Failed to connect to Relay Server";
        }
    }

    qDebug() << "";
    qDebug() << "[Main] Initializing QML engine...";

    QQmlApplicationEngine engine;
    
    // Register types with QML
    qmlRegisterType<RemoteAccessSystem::Mobile::ConnectionManager>("RemoteAccess", 1, 0, "ConnectionManager");
    qmlRegisterType<RemoteAccessSystem::Mobile::PCManager>("RemoteAccess", 1, 0, "PCManager");
    qmlRegisterType<RemoteAccessSystem::Mobile::FileManager>("RemoteAccess", 1, 0, "FileManager");
    qmlRegisterType<RemoteAccessSystem::Mobile::SettingsManager>("RemoteAccess", 1, 0, "SettingsManager");
    
    qDebug() << "[Main] ✓ QML types registered";
    
    // Expose instances to QML
    engine.rootContext()->setContextProperty("connectionManager", connectionManager);
    engine.rootContext()->setContextProperty("pcManager", pcManager);
    engine.rootContext()->setContextProperty("fileManager", fileManager);
    engine.rootContext()->setContextProperty("settingsManager", settingsManager);
    
    qDebug() << "[Main] ✓ Context properties set";

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qDebug() << "[Main] ERROR: Failed to load QML";
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    
    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qDebug() << "[Main] ERROR: No root objects loaded";
        return -1;
    }
    
    qDebug() << "[Main] ✓ QML loaded successfully";
    qDebug() << "";
    qDebug() << "===========================================";
    qDebug() << "Application Ready - Waiting for user input";
    qDebug() << "===========================================";
    qDebug() << "";

    return app.exec();
}
