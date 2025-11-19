/****************************************************************************
** Meta object code from reading C++ file 'pcmanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/pcmanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pcmanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9PCManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto PCManager::qt_create_metaobjectdata<qt_meta_tag_ZN9PCManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PCManager",
        "pcListUpdated",
        "",
        "pcList",
        "connectionEstablished",
        "pcId",
        "connectionFailed",
        "error",
        "connectedPcIdChanged",
        "isConnectedChanged",
        "clearPCList",
        "addPCToList",
        "hostname",
        "username",
        "isOnline",
        "onConnected",
        "onReadyRead",
        "onDisconnected",
        "onError",
        "QAbstractSocket::SocketError",
        "queryPCList",
        "relayServerAddress",
        "port",
        "connectToPC",
        "disconnectFromPC",
        "connectedPcId",
        "isConnected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'pcListUpdated'
        QtMocHelpers::SignalData<void(const QStringList &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 3 },
        }}),
        // Signal 'connectionEstablished'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'connectionFailed'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'connectedPcIdChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'isConnectedChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'clearPCList'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'addPCToList'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &, bool)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QString, 12 }, { QMetaType::QString, 13 }, { QMetaType::Bool, 14 },
        }}),
        // Slot 'onConnected'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnected'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 19, 7 },
        }}),
        // Method 'queryPCList'
        QtMocHelpers::MethodData<void(const QString &, int)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 21 }, { QMetaType::Int, 22 },
        }}),
        // Method 'queryPCList'
        QtMocHelpers::MethodData<void(const QString &)>(20, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 21 },
        }}),
        // Method 'connectToPC'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QString, 21 },
        }}),
        // Method 'disconnectFromPC'
        QtMocHelpers::MethodData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'connectedPcId'
        QtMocHelpers::PropertyData<QString>(25, QMetaType::QString, QMC::DefaultPropertyFlags, 3),
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(26, QMetaType::Bool, QMC::DefaultPropertyFlags, 4),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PCManager, qt_meta_tag_ZN9PCManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PCManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9PCManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9PCManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9PCManagerE_t>.metaTypes,
    nullptr
} };

void PCManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PCManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->pcListUpdated((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 1: _t->connectionEstablished((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->connectionFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->connectedPcIdChanged(); break;
        case 4: _t->isConnectedChanged(); break;
        case 5: _t->clearPCList(); break;
        case 6: _t->addPCToList((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[4]))); break;
        case 7: _t->onConnected(); break;
        case 8: _t->onReadyRead(); break;
        case 9: _t->onDisconnected(); break;
        case 10: _t->onError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 11: _t->queryPCList((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 12: _t->queryPCList((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->connectToPC((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->disconnectFromPC(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PCManager::*)(const QStringList & )>(_a, &PCManager::pcListUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PCManager::*)(const QString & )>(_a, &PCManager::connectionEstablished, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (PCManager::*)(const QString & )>(_a, &PCManager::connectionFailed, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (PCManager::*)()>(_a, &PCManager::connectedPcIdChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (PCManager::*)()>(_a, &PCManager::isConnectedChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (PCManager::*)()>(_a, &PCManager::clearPCList, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (PCManager::*)(const QString & , const QString & , const QString & , bool )>(_a, &PCManager::addPCToList, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->connectedPcId(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->isConnected(); break;
        default: break;
        }
    }
}

const QMetaObject *PCManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PCManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9PCManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PCManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PCManager::pcListUpdated(const QStringList & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void PCManager::connectionEstablished(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void PCManager::connectionFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void PCManager::connectedPcIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void PCManager::isConnectedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PCManager::clearPCList()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void PCManager::addPCToList(const QString & _t1, const QString & _t2, const QString & _t3, bool _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2, _t3, _t4);
}
QT_WARNING_POP
