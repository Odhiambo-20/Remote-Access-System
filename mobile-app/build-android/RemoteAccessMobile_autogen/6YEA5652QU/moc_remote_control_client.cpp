/****************************************************************************
** Meta object code from reading C++ file 'remote_control_client.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/remote_control_client.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'remote_control_client.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19RemoteControlClientE_t {};
} // unnamed namespace

template <> constexpr inline auto RemoteControlClient::qt_create_metaobjectdata<qt_meta_tag_ZN19RemoteControlClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RemoteControlClient",
        "connected",
        "",
        "disconnected",
        "connectedChanged",
        "sessionStarted",
        "frameReceived",
        "frameData",
        "error",
        "errorMessage",
        "handleData",
        "connectToPC",
        "pcId",
        "relayServer",
        "relayPort",
        "disconnect",
        "sendMouseMove",
        "x",
        "y",
        "sendMouseClick",
        "button",
        "sendKeyPress",
        "key"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'connectedChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sessionStarted'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'frameReceived'
        QtMocHelpers::SignalData<void(const QByteArray &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 7 },
        }}),
        // Signal 'error'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'handleData'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Method 'connectToPC'
        QtMocHelpers::MethodData<void(const QString &, const QString &, int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 13 }, { QMetaType::Int, 14 },
        }}),
        // Method 'disconnect'
        QtMocHelpers::MethodData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'sendMouseMove'
        QtMocHelpers::MethodData<void(int, int)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 17 }, { QMetaType::Int, 18 },
        }}),
        // Method 'sendMouseClick'
        QtMocHelpers::MethodData<void(int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 20 },
        }}),
        // Method 'sendKeyPress'
        QtMocHelpers::MethodData<void(int)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 22 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'connected'
        QtMocHelpers::PropertyData<bool>(1, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RemoteControlClient, qt_meta_tag_ZN19RemoteControlClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RemoteControlClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19RemoteControlClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19RemoteControlClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19RemoteControlClientE_t>.metaTypes,
    nullptr
} };

void RemoteControlClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RemoteControlClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectedChanged(); break;
        case 3: _t->sessionStarted(); break;
        case 4: _t->frameReceived((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 5: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->handleData(); break;
        case 7: _t->connectToPC((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 8: _t->disconnect(); break;
        case 9: _t->sendMouseMove((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 10: _t->sendMouseClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->sendKeyPress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RemoteControlClient::*)()>(_a, &RemoteControlClient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RemoteControlClient::*)()>(_a, &RemoteControlClient::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RemoteControlClient::*)()>(_a, &RemoteControlClient::connectedChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RemoteControlClient::*)()>(_a, &RemoteControlClient::sessionStarted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RemoteControlClient::*)(const QByteArray & )>(_a, &RemoteControlClient::frameReceived, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RemoteControlClient::*)(const QString & )>(_a, &RemoteControlClient::error, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isConnected(); break;
        default: break;
        }
    }
}

const QMetaObject *RemoteControlClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RemoteControlClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19RemoteControlClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RemoteControlClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void RemoteControlClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RemoteControlClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RemoteControlClient::connectedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RemoteControlClient::sessionStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RemoteControlClient::frameReceived(const QByteArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void RemoteControlClient::error(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
