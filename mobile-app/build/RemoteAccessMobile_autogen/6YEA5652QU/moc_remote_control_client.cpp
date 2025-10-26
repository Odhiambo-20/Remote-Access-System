/****************************************************************************
** Meta object code from reading C++ file 'remote_control_client.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/remote_control_client.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'remote_control_client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_RemoteControlClient_t {
    uint offsetsAndSizes[46];
    char stringdata0[20];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[17];
    char stringdata5[15];
    char stringdata6[14];
    char stringdata7[10];
    char stringdata8[6];
    char stringdata9[13];
    char stringdata10[11];
    char stringdata11[12];
    char stringdata12[5];
    char stringdata13[12];
    char stringdata14[10];
    char stringdata15[11];
    char stringdata16[14];
    char stringdata17[2];
    char stringdata18[2];
    char stringdata19[15];
    char stringdata20[7];
    char stringdata21[13];
    char stringdata22[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RemoteControlClient_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RemoteControlClient_t qt_meta_stringdata_RemoteControlClient = {
    {
        QT_MOC_LITERAL(0, 19),  // "RemoteControlClient"
        QT_MOC_LITERAL(20, 9),  // "connected"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 12),  // "disconnected"
        QT_MOC_LITERAL(44, 16),  // "connectedChanged"
        QT_MOC_LITERAL(61, 14),  // "sessionStarted"
        QT_MOC_LITERAL(76, 13),  // "frameReceived"
        QT_MOC_LITERAL(90, 9),  // "frameData"
        QT_MOC_LITERAL(100, 5),  // "error"
        QT_MOC_LITERAL(106, 12),  // "errorMessage"
        QT_MOC_LITERAL(119, 10),  // "handleData"
        QT_MOC_LITERAL(130, 11),  // "connectToPC"
        QT_MOC_LITERAL(142, 4),  // "pcId"
        QT_MOC_LITERAL(147, 11),  // "relayServer"
        QT_MOC_LITERAL(159, 9),  // "relayPort"
        QT_MOC_LITERAL(169, 10),  // "disconnect"
        QT_MOC_LITERAL(180, 13),  // "sendMouseMove"
        QT_MOC_LITERAL(194, 1),  // "x"
        QT_MOC_LITERAL(196, 1),  // "y"
        QT_MOC_LITERAL(198, 14),  // "sendMouseClick"
        QT_MOC_LITERAL(213, 6),  // "button"
        QT_MOC_LITERAL(220, 12),  // "sendKeyPress"
        QT_MOC_LITERAL(233, 3)   // "key"
    },
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
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RemoteControlClient[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       1,  116, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x06,    2 /* Public */,
       3,    0,   87,    2, 0x06,    3 /* Public */,
       4,    0,   88,    2, 0x06,    4 /* Public */,
       5,    0,   89,    2, 0x06,    5 /* Public */,
       6,    1,   90,    2, 0x06,    6 /* Public */,
       8,    1,   93,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    0,   96,    2, 0x08,   10 /* Private */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      11,    3,   97,    2, 0x02,   11 /* Public */,
      15,    0,  104,    2, 0x02,   15 /* Public */,
      16,    2,  105,    2, 0x02,   16 /* Public */,
      19,    1,  110,    2, 0x02,   19 /* Public */,
      21,    1,  113,    2, 0x02,   21 /* Public */,

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
       1, QMetaType::Bool, 0x00015001, uint(2), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject RemoteControlClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_RemoteControlClient.offsetsAndSizes,
    qt_meta_data_RemoteControlClient,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RemoteControlClient_t,
        // property 'connected'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RemoteControlClient, std::true_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sessionStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'frameReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handleData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectToPC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disconnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendMouseMove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'sendMouseClick'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'sendKeyPress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

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
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RemoteControlClient::*)();
            if (_t _q_method = &RemoteControlClient::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)();
            if (_t _q_method = &RemoteControlClient::disconnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)();
            if (_t _q_method = &RemoteControlClient::connectedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)();
            if (_t _q_method = &RemoteControlClient::sessionStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)(const QByteArray & );
            if (_t _q_method = &RemoteControlClient::frameReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (RemoteControlClient::*)(const QString & );
            if (_t _q_method = &RemoteControlClient::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<RemoteControlClient *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

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
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
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
