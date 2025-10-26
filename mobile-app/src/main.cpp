#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "connection_manager.h"
#include "pcmanager.h"
#include "filemanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Register C++ types
    qmlRegisterType<ConnectionManager>("RemoteAccess", 1, 0, "ConnectionManager");
    qmlRegisterType<FileManager>("RemoteAccess", 1, 0, "FileManager");

    QQmlApplicationEngine engine;

    // Create PCManager instance and expose to QML
    PCManager *pcManager = new PCManager(&app);
    engine.rootContext()->setContextProperty("pcManager", pcManager);

    const QUrl url(u"qrc:/RemoteAccess/qml/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);

    return app.exec();
}
