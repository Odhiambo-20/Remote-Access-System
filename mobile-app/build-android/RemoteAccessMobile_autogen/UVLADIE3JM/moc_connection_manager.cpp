/****************************************************************************
** Meta object code from reading C++ file 'connection_manager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/connection_manager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connection_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN17ConnectionManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto ConnectionManager::qt_create_metaobjectdata<qt_meta_tag_ZN17ConnectionManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ConnectionManager",
        "loginSuccess",
        "",
        "loginFailed",
        "error",
        "qrCodeScanned",
        "pcId",
        "username",
        "qrCodeParsed",
        "success",
        "message",
        "qrCodeError",
        "connectionStatusChanged",
        "status",
        "isConnectedChanged",
        "connected",
        "currentPCNameChanged",
        "name",
        "pcConnected",
        "pcDisconnected",
        "connectionError",
        "pcListUpdated",
        "onRelayConnected",
        "onRelayDisconnected",
        "onRelayReadyRead",
        "onRelayError",
        "QAbstractSocket::SocketError",
        "onConnectionTimeout",
        "setAccountServerAddress",
        "address",
        "getAccountServerAddress",
        "login",
        "password",
        "scanQRCode",
        "qrData",
        "parseQRCode",
        "getLastScannedPCInfo",
        "connectToPC",
        "disconnectFromPC",
        "connectViaScan",
        "connectionStatus",
        "isConnected",
        "currentPCName",
        "getSavedPCs",
        "savePC",
        "displayName",
        "removePC"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'loginSuccess'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'loginFailed'
        QtMocHelpers::SignalData<void(const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'qrCodeScanned'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'qrCodeParsed'
        QtMocHelpers::SignalData<void(bool, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 9 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'qrCodeError'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'connectionStatusChanged'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Signal 'isConnectedChanged'
        QtMocHelpers::SignalData<void(bool)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 15 },
        }}),
        // Signal 'currentPCNameChanged'
        QtMocHelpers::SignalData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Signal 'pcConnected'
        QtMocHelpers::SignalData<void(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'pcDisconnected'
        QtMocHelpers::SignalData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'connectionError'
        QtMocHelpers::SignalData<void(const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'pcListUpdated'
        QtMocHelpers::SignalData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onRelayConnected'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRelayDisconnected'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRelayReadyRead'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRelayError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 26, 4 },
        }}),
        // Slot 'onConnectionTimeout'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Method 'setAccountServerAddress'
        QtMocHelpers::MethodData<void(const QString &)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 29 },
        }}),
        // Method 'getAccountServerAddress'
        QtMocHelpers::MethodData<QString() const>(30, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'login'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 32 },
        }}),
        // Method 'scanQRCode'
        QtMocHelpers::MethodData<void(const QString &)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 34 },
        }}),
        // Method 'parseQRCode'
        QtMocHelpers::MethodData<bool(const QString &)>(35, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 34 },
        }}),
        // Method 'getLastScannedPCInfo'
        QtMocHelpers::MethodData<QString() const>(36, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'connectToPC'
        QtMocHelpers::MethodData<void()>(37, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'connectToPC'
        QtMocHelpers::MethodData<void(const QString &)>(37, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Method 'disconnectFromPC'
        QtMocHelpers::MethodData<void()>(38, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'connectViaScan'
        QtMocHelpers::MethodData<void(const QString &)>(39, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 34 },
        }}),
        // Method 'connectionStatus'
        QtMocHelpers::MethodData<QString() const>(40, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'isConnected'
        QtMocHelpers::MethodData<bool() const>(41, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'currentPCName'
        QtMocHelpers::MethodData<QString() const>(42, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'getSavedPCs'
        QtMocHelpers::MethodData<QStringList() const>(43, 2, QMC::AccessPublic, QMetaType::QStringList),
        // Method 'savePC'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(44, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 45 },
        }}),
        // Method 'removePC'
        QtMocHelpers::MethodData<void(const QString &)>(46, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'connectionStatus'
        QtMocHelpers::PropertyData<QString>(40, QMetaType::QString, QMC::DefaultPropertyFlags, 5),
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(41, QMetaType::Bool, QMC::DefaultPropertyFlags, 6),
        // property 'currentPCName'
        QtMocHelpers::PropertyData<QString>(42, QMetaType::QString, QMC::DefaultPropertyFlags, 7),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ConnectionManager, qt_meta_tag_ZN17ConnectionManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ConnectionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ConnectionManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ConnectionManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17ConnectionManagerE_t>.metaTypes,
    nullptr
} };

void ConnectionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ConnectionManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loginSuccess(); break;
        case 1: _t->loginFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->qrCodeScanned((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->qrCodeParsed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->qrCodeError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->connectionStatusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->isConnectedChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->currentPCNameChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->pcConnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->pcDisconnected(); break;
        case 10: _t->connectionError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->pcListUpdated(); break;
        case 12: _t->onRelayConnected(); break;
        case 13: _t->onRelayDisconnected(); break;
        case 14: _t->onRelayReadyRead(); break;
        case 15: _t->onRelayError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 16: _t->onConnectionTimeout(); break;
        case 17: _t->setAccountServerAddress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: { QString _r = _t->getAccountServerAddress();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 19: _t->login((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 20: _t->scanQRCode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: { bool _r = _t->parseQRCode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 22: { QString _r = _t->getLastScannedPCInfo();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 23: _t->connectToPC(); break;
        case 24: _t->connectToPC((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 25: _t->disconnectFromPC(); break;
        case 26: _t->connectViaScan((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: { QString _r = _t->connectionStatus();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 28: { bool _r = _t->isConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 29: { QString _r = _t->currentPCName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 30: { QStringList _r = _t->getSavedPCs();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 31: _t->savePC((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 32: _t->removePC((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)()>(_a, &ConnectionManager::loginSuccess, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & )>(_a, &ConnectionManager::loginFailed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & , const QString & )>(_a, &ConnectionManager::qrCodeScanned, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(bool , const QString & )>(_a, &ConnectionManager::qrCodeParsed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & )>(_a, &ConnectionManager::qrCodeError, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & )>(_a, &ConnectionManager::connectionStatusChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(bool )>(_a, &ConnectionManager::isConnectedChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & )>(_a, &ConnectionManager::currentPCNameChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & )>(_a, &ConnectionManager::pcConnected, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)()>(_a, &ConnectionManager::pcDisconnected, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & )>(_a, &ConnectionManager::connectionError, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)()>(_a, &ConnectionManager::pcListUpdated, 11))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->connectionStatus(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->isConnected(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->currentPCName(); break;
        default: break;
        }
    }
}

const QMetaObject *ConnectionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ConnectionManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ConnectionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ConnectionManager::loginSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConnectionManager::loginFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ConnectionManager::qrCodeScanned(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void ConnectionManager::qrCodeParsed(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void ConnectionManager::qrCodeError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ConnectionManager::connectionStatusChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ConnectionManager::isConnectedChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void ConnectionManager::currentPCNameChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void ConnectionManager::pcConnected(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void ConnectionManager::pcDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ConnectionManager::connectionError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void ConnectionManager::pcListUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}
QT_WARNING_POP
