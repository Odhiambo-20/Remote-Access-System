/****************************************************************************
** Meta object code from reading C++ file 'pcmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/pcmanager.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pcmanager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PCManager_t {
    uint offsetsAndSizes[54];
    char stringdata0[10];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[22];
    char stringdata5[5];
    char stringdata6[17];
    char stringdata7[6];
    char stringdata8[21];
    char stringdata9[19];
    char stringdata10[12];
    char stringdata11[12];
    char stringdata12[9];
    char stringdata13[9];
    char stringdata14[9];
    char stringdata15[12];
    char stringdata16[12];
    char stringdata17[15];
    char stringdata18[8];
    char stringdata19[29];
    char stringdata20[12];
    char stringdata21[19];
    char stringdata22[5];
    char stringdata23[12];
    char stringdata24[17];
    char stringdata25[14];
    char stringdata26[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PCManager_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PCManager_t qt_meta_stringdata_PCManager = {
    {
        QT_MOC_LITERAL(0, 9),  // "PCManager"
        QT_MOC_LITERAL(10, 13),  // "pcListUpdated"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 6),  // "pcList"
        QT_MOC_LITERAL(32, 21),  // "connectionEstablished"
        QT_MOC_LITERAL(54, 4),  // "pcId"
        QT_MOC_LITERAL(59, 16),  // "connectionFailed"
        QT_MOC_LITERAL(76, 5),  // "error"
        QT_MOC_LITERAL(82, 20),  // "connectedPcIdChanged"
        QT_MOC_LITERAL(103, 18),  // "isConnectedChanged"
        QT_MOC_LITERAL(122, 11),  // "clearPCList"
        QT_MOC_LITERAL(134, 11),  // "addPCToList"
        QT_MOC_LITERAL(146, 8),  // "hostname"
        QT_MOC_LITERAL(155, 8),  // "username"
        QT_MOC_LITERAL(164, 8),  // "isOnline"
        QT_MOC_LITERAL(173, 11),  // "onConnected"
        QT_MOC_LITERAL(185, 11),  // "onReadyRead"
        QT_MOC_LITERAL(197, 14),  // "onDisconnected"
        QT_MOC_LITERAL(212, 7),  // "onError"
        QT_MOC_LITERAL(220, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(249, 11),  // "queryPCList"
        QT_MOC_LITERAL(261, 18),  // "relayServerAddress"
        QT_MOC_LITERAL(280, 4),  // "port"
        QT_MOC_LITERAL(285, 11),  // "connectToPC"
        QT_MOC_LITERAL(297, 16),  // "disconnectFromPC"
        QT_MOC_LITERAL(314, 13),  // "connectedPcId"
        QT_MOC_LITERAL(328, 11)   // "isConnected"
    },
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
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PCManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       2,  145, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  104,    2, 0x06,    3 /* Public */,
       4,    1,  107,    2, 0x06,    5 /* Public */,
       6,    1,  110,    2, 0x06,    7 /* Public */,
       8,    0,  113,    2, 0x06,    9 /* Public */,
       9,    0,  114,    2, 0x06,   10 /* Public */,
      10,    0,  115,    2, 0x06,   11 /* Public */,
      11,    4,  116,    2, 0x06,   12 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    0,  125,    2, 0x08,   17 /* Private */,
      16,    0,  126,    2, 0x08,   18 /* Private */,
      17,    0,  127,    2, 0x08,   19 /* Private */,
      18,    1,  128,    2, 0x08,   20 /* Private */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      20,    2,  131,    2, 0x02,   22 /* Public */,
      20,    1,  136,    2, 0x22,   25 /* Public | MethodCloned */,
      23,    2,  139,    2, 0x02,   27 /* Public */,
      24,    0,  144,    2, 0x02,   30 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::Bool,    5,   12,   13,   14,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19,    7,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   21,   22,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,   21,
    QMetaType::Void,

 // properties: name, type, flags
      25, QMetaType::QString, 0x00015001, uint(3), 0,
      26, QMetaType::Bool, 0x00015001, uint(4), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject PCManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PCManager.offsetsAndSizes,
    qt_meta_data_PCManager,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PCManager_t,
        // property 'connectedPcId'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'isConnected'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PCManager, std::true_type>,
        // method 'pcListUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'connectionEstablished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'connectionFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'connectedPcIdChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'isConnectedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearPCList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addPCToList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'queryPCList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'queryPCList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'connectToPC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'disconnectFromPC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PCManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PCManager *>(_o);
        (void)_t;
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
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
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PCManager::*)(const QStringList & );
            if (_t _q_method = &PCManager::pcListUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PCManager::*)(const QString & );
            if (_t _q_method = &PCManager::connectionEstablished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PCManager::*)(const QString & );
            if (_t _q_method = &PCManager::connectionFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PCManager::*)();
            if (_t _q_method = &PCManager::connectedPcIdChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PCManager::*)();
            if (_t _q_method = &PCManager::isConnectedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PCManager::*)();
            if (_t _q_method = &PCManager::clearPCList; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (PCManager::*)(const QString & , const QString & , const QString & , bool );
            if (_t _q_method = &PCManager::addPCToList; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
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
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

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
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
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

// SIGNAL 5
void PCManager::clearPCList()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void PCManager::addPCToList(const QString & _t1, const QString & _t2, const QString & _t3, bool _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
