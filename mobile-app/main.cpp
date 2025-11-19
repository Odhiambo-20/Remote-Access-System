#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "connection_manager.h"
#include "pc_manager.h"
#include "file_manager.h"
#include "settings_manager.h"
#include "remote_control_client.h"
#include "pc_list_model.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    qmlRegisterType<ConnectionManager>("RemoteAccess", 1, 0, "ConnectionManager");
    qmlRegisterType<PCManager>("RemoteAccess", 1, 0, "PCManager");
    qmlRegisterType<FileManager>("RemoteAccess", 1, 0, "FileManager");
    qmlRegisterType<SettingsManager>("RemoteAccess", 1, 0, "SettingsManager");
    qmlRegisterType<RemoteControlClient>("RemoteAccess", 1, 0, "RemoteControlClient");
    
    PCListModel pcListModel;
    
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("pcListModel", &pcListModel);
    
    // Use the QML module URI path
    const QUrl url(QStringLiteral("qrc:/qt/qml/RemoteAccess/qml/main.qml"));
    engine.load(url);
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}
