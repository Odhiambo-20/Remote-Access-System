/****************************************************************************
** Meta object code from reading C++ file 'connection_manager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/connection_manager.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connection_manager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_ConnectionManager_t {
    uint offsetsAndSizes[94];
    char stringdata0[18];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[6];
    char stringdata5[14];
    char stringdata6[5];
    char stringdata7[9];
    char stringdata8[13];
    char stringdata9[8];
    char stringdata10[8];
    char stringdata11[12];
    char stringdata12[24];
    char stringdata13[7];
    char stringdata14[19];
    char stringdata15[10];
    char stringdata16[21];
    char stringdata17[5];
    char stringdata18[12];
    char stringdata19[15];
    char stringdata20[16];
    char stringdata21[14];
    char stringdata22[17];
    char stringdata23[20];
    char stringdata24[17];
    char stringdata25[13];
    char stringdata26[29];
    char stringdata27[20];
    char stringdata28[24];
    char stringdata29[8];
    char stringdata30[24];
    char stringdata31[6];
    char stringdata32[9];
    char stringdata33[11];
    char stringdata34[7];
    char stringdata35[12];
    char stringdata36[21];
    char stringdata37[12];
    char stringdata38[17];
    char stringdata39[15];
    char stringdata40[17];
    char stringdata41[12];
    char stringdata42[14];
    char stringdata43[12];
    char stringdata44[7];
    char stringdata45[12];
    char stringdata46[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ConnectionManager_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ConnectionManager_t qt_meta_stringdata_ConnectionManager = {
    {
        QT_MOC_LITERAL(0, 17),  // "ConnectionManager"
        QT_MOC_LITERAL(18, 12),  // "loginSuccess"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 11),  // "loginFailed"
        QT_MOC_LITERAL(44, 5),  // "error"
        QT_MOC_LITERAL(50, 13),  // "qrCodeScanned"
        QT_MOC_LITERAL(64, 4),  // "pcId"
        QT_MOC_LITERAL(69, 8),  // "username"
        QT_MOC_LITERAL(78, 12),  // "qrCodeParsed"
        QT_MOC_LITERAL(91, 7),  // "success"
        QT_MOC_LITERAL(99, 7),  // "message"
        QT_MOC_LITERAL(107, 11),  // "qrCodeError"
        QT_MOC_LITERAL(119, 23),  // "connectionStatusChanged"
        QT_MOC_LITERAL(143, 6),  // "status"
        QT_MOC_LITERAL(150, 18),  // "isConnectedChanged"
        QT_MOC_LITERAL(169, 9),  // "connected"
        QT_MOC_LITERAL(179, 20),  // "currentPCNameChanged"
        QT_MOC_LITERAL(200, 4),  // "name"
        QT_MOC_LITERAL(205, 11),  // "pcConnected"
        QT_MOC_LITERAL(217, 14),  // "pcDisconnected"
        QT_MOC_LITERAL(232, 15),  // "connectionError"
        QT_MOC_LITERAL(248, 13),  // "pcListUpdated"
        QT_MOC_LITERAL(262, 16),  // "onRelayConnected"
        QT_MOC_LITERAL(279, 19),  // "onRelayDisconnected"
        QT_MOC_LITERAL(299, 16),  // "onRelayReadyRead"
        QT_MOC_LITERAL(316, 12),  // "onRelayError"
        QT_MOC_LITERAL(329, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(358, 19),  // "onConnectionTimeout"
        QT_MOC_LITERAL(378, 23),  // "setAccountServerAddress"
        QT_MOC_LITERAL(402, 7),  // "address"
        QT_MOC_LITERAL(410, 23),  // "getAccountServerAddress"
        QT_MOC_LITERAL(434, 5),  // "login"
        QT_MOC_LITERAL(440, 8),  // "password"
        QT_MOC_LITERAL(449, 10),  // "scanQRCode"
        QT_MOC_LITERAL(460, 6),  // "qrData"
        QT_MOC_LITERAL(467, 11),  // "parseQRCode"
        QT_MOC_LITERAL(479, 20),  // "getLastScannedPCInfo"
        QT_MOC_LITERAL(500, 11),  // "connectToPC"
        QT_MOC_LITERAL(512, 16),  // "disconnectFromPC"
        QT_MOC_LITERAL(529, 14),  // "connectViaScan"
        QT_MOC_LITERAL(544, 16),  // "connectionStatus"
        QT_MOC_LITERAL(561, 11),  // "isConnected"
        QT_MOC_LITERAL(573, 13),  // "currentPCName"
        QT_MOC_LITERAL(587, 11),  // "getSavedPCs"
        QT_MOC_LITERAL(599, 6),  // "savePC"
        QT_MOC_LITERAL(606, 11),  // "displayName"
        QT_MOC_LITERAL(618, 8)   // "removePC"
    },
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
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ConnectionManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       3,  289, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  212,    2, 0x06,    4 /* Public */,
       3,    1,  213,    2, 0x06,    5 /* Public */,
       5,    2,  216,    2, 0x06,    7 /* Public */,
       8,    2,  221,    2, 0x06,   10 /* Public */,
      11,    1,  226,    2, 0x06,   13 /* Public */,
      12,    1,  229,    2, 0x06,   15 /* Public */,
      14,    1,  232,    2, 0x06,   17 /* Public */,
      16,    1,  235,    2, 0x06,   19 /* Public */,
      18,    1,  238,    2, 0x06,   21 /* Public */,
      19,    0,  241,    2, 0x06,   23 /* Public */,
      20,    1,  242,    2, 0x06,   24 /* Public */,
      21,    0,  245,    2, 0x06,   26 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      22,    0,  246,    2, 0x08,   27 /* Private */,
      23,    0,  247,    2, 0x08,   28 /* Private */,
      24,    0,  248,    2, 0x08,   29 /* Private */,
      25,    1,  249,    2, 0x08,   30 /* Private */,
      27,    0,  252,    2, 0x08,   32 /* Private */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      28,    1,  253,    2, 0x02,   33 /* Public */,
      30,    0,  256,    2, 0x102,   35 /* Public | MethodIsConst  */,
      31,    2,  257,    2, 0x02,   36 /* Public */,
      33,    1,  262,    2, 0x02,   39 /* Public */,
      35,    1,  265,    2, 0x02,   41 /* Public */,
      36,    0,  268,    2, 0x102,   43 /* Public | MethodIsConst  */,
      37,    0,  269,    2, 0x02,   44 /* Public */,
      37,    1,  270,    2, 0x02,   45 /* Public */,
      38,    0,  273,    2, 0x02,   47 /* Public */,
      39,    1,  274,    2, 0x02,   48 /* Public */,
      40,    0,  277,    2, 0x102,   50 /* Public | MethodIsConst  */,
      41,    0,  278,    2, 0x102,   51 /* Public | MethodIsConst  */,
      42,    0,  279,    2, 0x102,   52 /* Public | MethodIsConst  */,
      43,    0,  280,    2, 0x102,   53 /* Public | MethodIsConst  */,
      44,    2,  281,    2, 0x02,   54 /* Public */,
      46,    1,  286,    2, 0x02,   57 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    7,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    9,   10,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 26,    4,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::QString,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,   32,
    QMetaType::Void, QMetaType::QString,   34,
    QMetaType::Bool, QMetaType::QString,   34,
    QMetaType::QString,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   34,
    QMetaType::QString,
    QMetaType::Bool,
    QMetaType::QString,
    QMetaType::QStringList,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,   45,
    QMetaType::Void, QMetaType::QString,    6,

 // properties: name, type, flags
      40, QMetaType::QString, 0x00015001, uint(5), 0,
      41, QMetaType::Bool, 0x00015001, uint(6), 0,
      42, QMetaType::QString, 0x00015001, uint(7), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject ConnectionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ConnectionManager.offsetsAndSizes,
    qt_meta_data_ConnectionManager,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ConnectionManager_t,
        // property 'connectionStatus'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'isConnected'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'currentPCName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ConnectionManager, std::true_type>,
        // method 'loginSuccess'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loginFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'qrCodeScanned'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'qrCodeParsed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'qrCodeError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'connectionStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'isConnectedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'currentPCNameChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'pcConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'pcDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectionError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'pcListUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRelayConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRelayDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRelayReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRelayError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'onConnectionTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setAccountServerAddress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'getAccountServerAddress'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'login'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'scanQRCode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'parseQRCode'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'getLastScannedPCInfo'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'connectToPC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectToPC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'disconnectFromPC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectViaScan'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'connectionStatus'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'isConnected'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'currentPCName'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'getSavedPCs'
        QtPrivate::TypeAndForceComplete<QStringList, std::false_type>,
        // method 'savePC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'removePC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ConnectionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConnectionManager *>(_o);
        (void)_t;
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
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
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConnectionManager::*)();
            if (_t _q_method = &ConnectionManager::loginSuccess; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(const QString & );
            if (_t _q_method = &ConnectionManager::loginFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(const QString & , const QString & );
            if (_t _q_method = &ConnectionManager::qrCodeScanned; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(bool , const QString & );
            if (_t _q_method = &ConnectionManager::qrCodeParsed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(const QString & );
            if (_t _q_method = &ConnectionManager::qrCodeError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(const QString & );
            if (_t _q_method = &ConnectionManager::connectionStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(bool );
            if (_t _q_method = &ConnectionManager::isConnectedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(const QString & );
            if (_t _q_method = &ConnectionManager::currentPCNameChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(const QString & );
            if (_t _q_method = &ConnectionManager::pcConnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)();
            if (_t _q_method = &ConnectionManager::pcDisconnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)(const QString & );
            if (_t _q_method = &ConnectionManager::connectionError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ConnectionManager::*)();
            if (_t _q_method = &ConnectionManager::pcListUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ConnectionManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->connectionStatus(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->currentPCName(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *ConnectionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ConnectionManager.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
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
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ConnectionManager::qrCodeScanned(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ConnectionManager::qrCodeParsed(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ConnectionManager::qrCodeError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ConnectionManager::connectionStatusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ConnectionManager::isConnectedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ConnectionManager::currentPCNameChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ConnectionManager::pcConnected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ConnectionManager::pcDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ConnectionManager::connectionError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ConnectionManager::pcListUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
