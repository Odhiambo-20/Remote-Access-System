#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "connection_manager.h"
#include "pcmanager.h"
#include "filemanager.h"
#include "settings_manager.h"
#include "remote_control_client.h"
#include "pc_list_model.h"

int main(int argc, char *argv[])
{
    try {
        QGuiApplication app(argc, argv);

        qDebug() << "Starting Remote Access Mobile App...";

        // Register types
        qmlRegisterType<ConnectionManager>("RemoteAccess", 1, 0, "ConnectionManager");
        qmlRegisterType<PCManager>("RemoteAccess", 1, 0, "PCManager");
        qmlRegisterType<FileManager>("RemoteAccess", 1, 0, "FileManager");
        qmlRegisterType<SettingsManager>("RemoteAccess", 1, 0, "SettingsManager");
        qmlRegisterType<RemoteControlClient>("RemoteAccess", 1, 0, "RemoteControlClient");
        qmlRegisterType<PCListModel>("RemoteAccess", 1, 0, "PCListModel");

        qDebug() << "QML types registered successfully";

        QQmlApplicationEngine engine;
        
        // Add import path
        engine.addImportPath("qrc:/");
        
        const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
        
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                qCritical() << "Failed to load QML file!";
                QCoreApplication::exit(-1);
            } else {
                qDebug() << "QML loaded successfully";
            }
        }, Qt::QueuedConnection);
        
        qDebug() << "Loading QML...";
        engine.load(url);

        if (engine.rootObjects().isEmpty()) {
            qCritical() << "No root objects found!";
            return -1;
        }

        qDebug() << "App started successfully, entering event loop";
        return app.exec();
        
    } catch (const std::exception& e) {
        qCritical() << "Exception in main:" << e.what();
        return -1;
    } catch (...) {
        qCritical() << "Unknown exception in main!";
        return -1;
    }
}
