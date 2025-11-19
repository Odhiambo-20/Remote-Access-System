/****************************************************************************
** Meta object code from reading C++ file 'pcmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/pcmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pcmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PCManager_t {
    QByteArrayData data[22];
    char stringdata0[289];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PCManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PCManager_t qt_meta_stringdata_PCManager = {
    {
QT_MOC_LITERAL(0, 0, 9), // "PCManager"
QT_MOC_LITERAL(1, 10, 13), // "pcListUpdated"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 6), // "pcList"
QT_MOC_LITERAL(4, 32, 21), // "connectionEstablished"
QT_MOC_LITERAL(5, 54, 4), // "pcId"
QT_MOC_LITERAL(6, 59, 16), // "connectionFailed"
QT_MOC_LITERAL(7, 76, 5), // "error"
QT_MOC_LITERAL(8, 82, 20), // "connectedPcIdChanged"
QT_MOC_LITERAL(9, 103, 18), // "isConnectedChanged"
QT_MOC_LITERAL(10, 122, 11), // "onConnected"
QT_MOC_LITERAL(11, 134, 11), // "onReadyRead"
QT_MOC_LITERAL(12, 146, 14), // "onDisconnected"
QT_MOC_LITERAL(13, 161, 7), // "onError"
QT_MOC_LITERAL(14, 169, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(15, 198, 11), // "queryPCList"
QT_MOC_LITERAL(16, 210, 18), // "relayServerAddress"
QT_MOC_LITERAL(17, 229, 4), // "port"
QT_MOC_LITERAL(18, 234, 11), // "connectToPC"
QT_MOC_LITERAL(19, 246, 16), // "disconnectFromPC"
QT_MOC_LITERAL(20, 263, 13), // "connectedPcId"
QT_MOC_LITERAL(21, 277, 11) // "isConnected"

    },
    "PCManager\0pcListUpdated\0\0pcList\0"
    "connectionEstablished\0pcId\0connectionFailed\0"
    "error\0connectedPcIdChanged\0"
    "isConnectedChanged\0onConnected\0"
    "onReadyRead\0onDisconnected\0onError\0"
    "QAbstractSocket::SocketError\0queryPCList\0"
    "relayServerAddress\0port\0connectToPC\0"
    "disconnectFromPC\0connectedPcId\0"
    "isConnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PCManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       2,  110, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    1,   82,    2, 0x06 /* Public */,
       6,    1,   85,    2, 0x06 /* Public */,
       8,    0,   88,    2, 0x06 /* Public */,
       9,    0,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   90,    2, 0x08 /* Private */,
      11,    0,   91,    2, 0x08 /* Private */,
      12,    0,   92,    2, 0x08 /* Private */,
      13,    1,   93,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      15,    2,   96,    2, 0x02 /* Public */,
      15,    1,  101,    2, 0x22 /* Public | MethodCloned */,
      18,    2,  104,    2, 0x02 /* Public */,
      19,    0,  109,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,    7,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   16,   17,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,   16,
    QMetaType::Void,

 // properties: name, type, flags
      20, QMetaType::QString, 0x00495001,
      21, QMetaType::Bool, 0x00495001,

 // properties: notify_signal_id
       3,
       4,

       0        // eod
};

void PCManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PCManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pcListUpdated((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: _t->connectionEstablished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->connectionFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->connectedPcIdChanged(); break;
        case 4: _t->isConnectedChanged(); break;
        case 5: _t->onConnected(); break;
        case 6: _t->onReadyRead(); break;
        case 7: _t->onDisconnected(); break;
        case 8: _t->onError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 9: _t->queryPCList((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->queryPCList((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->connectToPC((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: _t->disconnectFromPC(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PCManager::*)(const QStringList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PCManager::pcListUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PCManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PCManager::connectionEstablished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PCManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PCManager::connectionFailed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PCManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PCManager::connectedPcIdChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PCManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PCManager::isConnectedChanged)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<PCManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->connectedPcId(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject PCManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PCManager.data,
    qt_meta_data_PCManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PCManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PCManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PCManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PCManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PCManager::pcListUpdated(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PCManager::connectionEstablished(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PCManager::connectionFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
