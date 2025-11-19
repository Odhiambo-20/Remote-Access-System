#include <QtQml/qqmlprivate.h>
#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

namespace QmlCacheGeneratedCode {
namespace _0x5f_RemoteAccess_qml_main_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f_RemoteAccess_qml_FileExplorerView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f_RemoteAccess_qml_PCListView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f_RemoteAccess_qml_AddPCView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f_RemoteAccess_qml_SettingsView_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f_RemoteAccess_qml_components_FileItem_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f_RemoteAccess_qml_components_PCCard_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _0x5f_RemoteAccess_qml_components_ProgressBar_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}

}
namespace {
struct Registry {
    Registry();
    ~Registry();
    QHash<QString, const QQmlPrivate::CachedQmlUnit*> resourcePathToCachedUnit;
    static const QQmlPrivate::CachedQmlUnit *lookupCachedUnit(const QUrl &url);
};

Q_GLOBAL_STATIC(Registry, unitRegistry)


Registry::Registry() {
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/main.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_main_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/FileExplorerView.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_FileExplorerView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/PCListView.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_PCListView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/AddPCView.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_AddPCView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/SettingsView.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_SettingsView_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/components/FileItem.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_components_FileItem_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/components/PCCard.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_components_PCCard_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/RemoteAccess/qml/components/ProgressBar.qml"), &QmlCacheGeneratedCode::_0x5f_RemoteAccess_qml_components_ProgressBar_qml::unit);
    QQmlPrivate::RegisterQmlUnitCacheHook registration;
    registration.structVersion = 0;
    registration.lookupCachedQmlUnit = &lookupCachedUnit;
    QQmlPrivate::qmlregister(QQmlPrivate::QmlUnitCacheHookRegistration, &registration);
}

Registry::~Registry() {
    QQmlPrivate::qmlunregister(QQmlPrivate::QmlUnitCacheHookRegistration, quintptr(&lookupCachedUnit));
}

const QQmlPrivate::CachedQmlUnit *Registry::lookupCachedUnit(const QUrl &url) {
    if (url.scheme() != QLatin1String("qrc"))
        return nullptr;
    QString resourcePath = QDir::cleanPath(url.path());
    if (resourcePath.isEmpty())
        return nullptr;
    if (!resourcePath.startsWith(QLatin1Char('/')))
        resourcePath.prepend(QLatin1Char('/'));
    return unitRegistry()->resourcePathToCachedUnit.value(resourcePath, nullptr);
}
}
int QT_MANGLE_NAMESPACE(qInitResources_qmlcache_RemoteAccessMobile)() {
    ::unitRegistry();
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_qmlcache_RemoteAccessMobile))
int QT_MANGLE_NAMESPACE(qCleanupResources_qmlcache_RemoteAccessMobile)() {
    return 1;
}
