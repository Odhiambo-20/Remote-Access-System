/****************************************************************************
** Meta object code from reading C++ file 'remote_control_client.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/remote_control_client.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'remote_control_client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RemoteControlClient_t {
    QByteArrayData data[23];
    char stringdata0[237];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RemoteControlClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RemoteControlClient_t qt_meta_stringdata_RemoteControlClient = {
    {
QT_MOC_LITERAL(0, 0, 19), // "RemoteControlClient"
QT_MOC_LITERAL(1, 20, 9), // "connected"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "disconnected"
QT_MOC_LITERAL(4, 44, 16), // "connectedChanged"
QT_MOC_LITERAL(5, 61, 14), // "sessionStarted"
QT_MOC_LITERAL(6, 76, 13), // "frameReceived"
QT_MOC_LITERAL(7, 90, 9), // "frameData"
QT_MOC_LITERAL(8, 100, 5), // "error"
QT_MOC_LITERAL(9, 106, 12), // "errorMessage"
QT_MOC_LITERAL(10, 119, 10), // "handleData"
QT_MOC_LITERAL(11, 130, 11), // "connectToPC"
QT_MOC_LITERAL(12, 142, 4), // "pcId"
QT_MOC_LITERAL(13, 147, 11), // "relayServer"
QT_MOC_LITERAL(14, 159, 9), // "relayPort"
QT_MOC_LITERAL(15, 169, 10), // "disconnect"
QT_MOC_LITERAL(16, 180, 13), // "sendMouseMove"
QT_MOC_LITERAL(17, 194, 1), // "x"
QT_MOC_LITERAL(18, 196, 1), // "y"
QT_MOC_LITERAL(19, 198, 14), // "sendMouseClick"
QT_MOC_LITERAL(20, 213, 6), // "button"
QT_MOC_LITERAL(21, 220, 12), // "sendKeyPress"
QT_MOC_LITERAL(22, 233, 3) // "key"

    },
    "RemoteControlClient\0connected\0\0"
    "disconnected\0connectedChanged\0"
    "sessionStarted\0frameReceived\0frameData\0"
    "error\0errorMessage\0handleData\0connectToPC\0"
    "pcId\0relayServer\0relayPort\0disconnect\0"
    "sendMouseMove\0x\0y\0sendMouseClick\0"
    "button\0sendKeyPress\0key"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RemoteControlClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       1,  104, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,
       4,    0,   76,    2, 0x06 /* Public */,
       5,    0,   77,    2, 0x06 /* Public */,
       6,    1,   78,    2, 0x06 /* Public */,
       8,    1,   81,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   84,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      11,    3,   85,    2, 0x02 /* Public */,
      15,    0,   92,    2, 0x02 /* Public */,
      16,    2,   93,    2, 0x02 /* Public */,
      19,    1,   98,    2, 0x02 /* Public */,
      21,    1,  101,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,   12,   13,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   17,   18,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::Int,   22,

 // properties: name, type, flags
       1, QMetaType::Bool, 0x00495001,

 // properties: notify_signal_id
       2,

       0        // eod
};

void RemoteControlClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RemoteControlClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectedChanged(); break;
        case 3: _t->sessionStarted(); break;
        case 4: _t->frameReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 5: _t->error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->handleData(); break;
        case 7: _t->connectToPC((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 8: _t->disconnect(); break;
        case 9: _t->sendMouseMove((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->sendMouseClick((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->sendKeyPress((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RemoteControlClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RemoteControlClient::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RemoteControlClient::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RemoteControlClient::connectedChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RemoteControlClient::sessionStarted)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RemoteControlClient::frameReceived)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RemoteControlClient::error)) {
                *result = 5;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<RemoteControlClient *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject RemoteControlClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_RemoteControlClient.data,
    qt_meta_data_RemoteControlClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RemoteControlClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RemoteControlClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RemoteControlClient.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
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
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void RemoteControlClient::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
