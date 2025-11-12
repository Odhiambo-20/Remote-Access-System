/****************************************************************************
** Meta object code from reading C++ file 'filemanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/filemanager.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filemanager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_FileManager_t {
    uint offsetsAndSizes[146];
    char stringdata0[12];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[17];
    char stringdata5[6];
    char stringdata6[16];
    char stringdata7[8];
    char stringdata8[22];
    char stringdata9[6];
    char stringdata10[14];
    char stringdata11[14];
    char stringdata12[5];
    char stringdata13[5];
    char stringdata14[5];
    char stringdata15[5];
    char stringdata16[6];
    char stringdata17[14];
    char stringdata18[15];
    char stringdata19[8];
    char stringdata20[8];
    char stringdata21[15];
    char stringdata22[8];
    char stringdata23[22];
    char stringdata24[19];
    char stringdata25[17];
    char stringdata26[18];
    char stringdata27[10];
    char stringdata28[19];
    char stringdata29[4];
    char stringdata30[17];
    char stringdata31[18];
    char stringdata32[11];
    char stringdata33[9];
    char stringdata34[10];
    char stringdata35[14];
    char stringdata36[12];
    char stringdata37[19];
    char stringdata38[12];
    char stringdata39[12];
    char stringdata40[15];
    char stringdata41[8];
    char stringdata42[29];
    char stringdata43[12];
    char stringdata44[5];
    char stringdata45[12];
    char stringdata46[16];
    char stringdata47[14];
    char stringdata48[11];
    char stringdata49[11];
    char stringdata50[11];
    char stringdata51[18];
    char stringdata52[12];
    char stringdata53[17];
    char stringdata54[16];
    char stringdata55[9];
    char stringdata56[7];
    char stringdata57[7];
    char stringdata58[11];
    char stringdata59[13];
    char stringdata60[22];
    char stringdata61[9];
    char stringdata62[11];
    char stringdata63[16];
    char stringdata64[9];
    char stringdata65[11];
    char stringdata66[8];
    char stringdata67[8];
    char stringdata68[13];
    char stringdata69[11];
    char stringdata70[7];
    char stringdata71[12];
    char stringdata72[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_FileManager_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_FileManager_t qt_meta_stringdata_FileManager = {
    {
        QT_MOC_LITERAL(0, 11),  // "FileManager"
        QT_MOC_LITERAL(12, 9),  // "connected"
        QT_MOC_LITERAL(22, 0),  // ""
        QT_MOC_LITERAL(23, 12),  // "disconnected"
        QT_MOC_LITERAL(36, 16),  // "connectionFailed"
        QT_MOC_LITERAL(53, 5),  // "error"
        QT_MOC_LITERAL(59, 15),  // "directoryListed"
        QT_MOC_LITERAL(75, 7),  // "entries"
        QT_MOC_LITERAL(83, 21),  // "directoryListReceived"
        QT_MOC_LITERAL(105, 5),  // "files"
        QT_MOC_LITERAL(111, 13),  // "clearFileList"
        QT_MOC_LITERAL(125, 13),  // "addFileToList"
        QT_MOC_LITERAL(139, 4),  // "name"
        QT_MOC_LITERAL(144, 4),  // "path"
        QT_MOC_LITERAL(149, 4),  // "type"
        QT_MOC_LITERAL(154, 4),  // "size"
        QT_MOC_LITERAL(159, 5),  // "isDir"
        QT_MOC_LITERAL(165, 13),  // "uploadStarted"
        QT_MOC_LITERAL(179, 14),  // "uploadFinished"
        QT_MOC_LITERAL(194, 7),  // "success"
        QT_MOC_LITERAL(202, 7),  // "message"
        QT_MOC_LITERAL(210, 14),  // "uploadProgress"
        QT_MOC_LITERAL(225, 7),  // "percent"
        QT_MOC_LITERAL(233, 21),  // "uploadProgressChanged"
        QT_MOC_LITERAL(255, 18),  // "isUploadingChanged"
        QT_MOC_LITERAL(274, 16),  // "downloadProgress"
        QT_MOC_LITERAL(291, 17),  // "downloadCompleted"
        QT_MOC_LITERAL(309, 9),  // "localPath"
        QT_MOC_LITERAL(319, 18),  // "shareLinkGenerated"
        QT_MOC_LITERAL(338, 3),  // "url"
        QT_MOC_LITERAL(342, 16),  // "shareLinkCreated"
        QT_MOC_LITERAL(359, 17),  // "shareUrlGenerated"
        QT_MOC_LITERAL(377, 10),  // "fileOpened"
        QT_MOC_LITERAL(388, 8),  // "filePath"
        QT_MOC_LITERAL(397, 9),  // "completed"
        QT_MOC_LITERAL(407, 13),  // "errorOccurred"
        QT_MOC_LITERAL(421, 11),  // "busyChanged"
        QT_MOC_LITERAL(433, 18),  // "currentPathChanged"
        QT_MOC_LITERAL(452, 11),  // "onConnected"
        QT_MOC_LITERAL(464, 11),  // "onReadyRead"
        QT_MOC_LITERAL(476, 14),  // "onDisconnected"
        QT_MOC_LITERAL(491, 7),  // "onError"
        QT_MOC_LITERAL(499, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(528, 11),  // "connectToPC"
        QT_MOC_LITERAL(540, 4),  // "pcId"
        QT_MOC_LITERAL(545, 11),  // "relayServer"
        QT_MOC_LITERAL(557, 15),  // "browseDirectory"
        QT_MOC_LITERAL(573, 13),  // "listDirectory"
        QT_MOC_LITERAL(587, 10),  // "uploadFile"
        QT_MOC_LITERAL(598, 10),  // "remotePath"
        QT_MOC_LITERAL(609, 10),  // "deleteFile"
        QT_MOC_LITERAL(620, 17),  // "generateShareLink"
        QT_MOC_LITERAL(638, 11),  // "expiryHours"
        QT_MOC_LITERAL(650, 16),  // "generateShareUrl"
        QT_MOC_LITERAL(667, 15),  // "createShareLink"
        QT_MOC_LITERAL(683, 8),  // "openFile"
        QT_MOC_LITERAL(692, 6),  // "goBack"
        QT_MOC_LITERAL(699, 6),  // "goHome"
        QT_MOC_LITERAL(706, 10),  // "disconnect"
        QT_MOC_LITERAL(717, 12),  // "downloadFile"
        QT_MOC_LITERAL(730, 21),  // "downloadMultipleFiles"
        QT_MOC_LITERAL(752, 8),  // "copyFile"
        QT_MOC_LITERAL(761, 10),  // "sourcePath"
        QT_MOC_LITERAL(772, 15),  // "destinationPath"
        QT_MOC_LITERAL(788, 8),  // "moveFile"
        QT_MOC_LITERAL(797, 10),  // "renameFile"
        QT_MOC_LITERAL(808, 7),  // "oldPath"
        QT_MOC_LITERAL(816, 7),  // "newName"
        QT_MOC_LITERAL(824, 12),  // "createFolder"
        QT_MOC_LITERAL(837, 10),  // "folderPath"
        QT_MOC_LITERAL(848, 6),  // "isBusy"
        QT_MOC_LITERAL(855, 11),  // "isUploading"
        QT_MOC_LITERAL(867, 11)   // "currentPath"
    },
    "FileManager",
    "connected",
    "",
    "disconnected",
    "connectionFailed",
    "error",
    "directoryListed",
    "entries",
    "directoryListReceived",
    "files",
    "clearFileList",
    "addFileToList",
    "name",
    "path",
    "type",
    "size",
    "isDir",
    "uploadStarted",
    "uploadFinished",
    "success",
    "message",
    "uploadProgress",
    "percent",
    "uploadProgressChanged",
    "isUploadingChanged",
    "downloadProgress",
    "downloadCompleted",
    "localPath",
    "shareLinkGenerated",
    "url",
    "shareLinkCreated",
    "shareUrlGenerated",
    "fileOpened",
    "filePath",
    "completed",
    "errorOccurred",
    "busyChanged",
    "currentPathChanged",
    "onConnected",
    "onReadyRead",
    "onDisconnected",
    "onError",
    "QAbstractSocket::SocketError",
    "connectToPC",
    "pcId",
    "relayServer",
    "browseDirectory",
    "listDirectory",
    "uploadFile",
    "remotePath",
    "deleteFile",
    "generateShareLink",
    "expiryHours",
    "generateShareUrl",
    "createShareLink",
    "openFile",
    "goBack",
    "goHome",
    "disconnect",
    "downloadFile",
    "downloadMultipleFiles",
    "copyFile",
    "sourcePath",
    "destinationPath",
    "moveFile",
    "renameFile",
    "oldPath",
    "newName",
    "createFolder",
    "folderPath",
    "isBusy",
    "isUploading",
    "currentPath"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_FileManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   14, // methods
       4,  422, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      23,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  290,    2, 0x06,    5 /* Public */,
       3,    0,  291,    2, 0x06,    6 /* Public */,
       4,    1,  292,    2, 0x06,    7 /* Public */,
       6,    1,  295,    2, 0x06,    9 /* Public */,
       8,    1,  298,    2, 0x06,   11 /* Public */,
      10,    0,  301,    2, 0x06,   13 /* Public */,
      11,    5,  302,    2, 0x06,   14 /* Public */,
      17,    0,  313,    2, 0x06,   20 /* Public */,
      18,    2,  314,    2, 0x06,   21 /* Public */,
      21,    1,  319,    2, 0x06,   24 /* Public */,
      23,    0,  322,    2, 0x06,   26 /* Public */,
      24,    0,  323,    2, 0x06,   27 /* Public */,
      25,    1,  324,    2, 0x06,   28 /* Public */,
      26,    1,  327,    2, 0x06,   30 /* Public */,
      28,    1,  330,    2, 0x06,   32 /* Public */,
      30,    1,  333,    2, 0x06,   34 /* Public */,
      31,    1,  336,    2, 0x06,   36 /* Public */,
      32,    1,  339,    2, 0x06,   38 /* Public */,
      34,    0,  342,    2, 0x06,   40 /* Public */,
       5,    1,  343,    2, 0x06,   41 /* Public */,
      35,    1,  346,    2, 0x06,   43 /* Public */,
      36,    0,  349,    2, 0x06,   45 /* Public */,
      37,    0,  350,    2, 0x06,   46 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      38,    0,  351,    2, 0x08,   47 /* Private */,
      39,    0,  352,    2, 0x08,   48 /* Private */,
      40,    0,  353,    2, 0x08,   49 /* Private */,
      41,    1,  354,    2, 0x08,   50 /* Private */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      43,    2,  357,    2, 0x02,   52 /* Public */,
      46,    1,  362,    2, 0x02,   55 /* Public */,
      47,    1,  365,    2, 0x02,   57 /* Public */,
      48,    2,  368,    2, 0x02,   59 /* Public */,
      50,    1,  373,    2, 0x02,   62 /* Public */,
      51,    2,  376,    2, 0x02,   64 /* Public */,
      53,    1,  381,    2, 0x02,   67 /* Public */,
      54,    1,  384,    2, 0x02,   69 /* Public */,
      55,    1,  387,    2, 0x02,   71 /* Public */,
      56,    0,  390,    2, 0x02,   73 /* Public */,
      57,    0,  391,    2, 0x02,   74 /* Public */,
      58,    0,  392,    2, 0x02,   75 /* Public */,
      59,    2,  393,    2, 0x02,   76 /* Public */,
      59,    1,  398,    2, 0x22,   79 /* Public | MethodCloned */,
      60,    1,  401,    2, 0x02,   81 /* Public */,
      61,    2,  404,    2, 0x02,   83 /* Public */,
      64,    2,  409,    2, 0x02,   86 /* Public */,
      65,    2,  414,    2, 0x02,   89 /* Public */,
      68,    1,  419,    2, 0x02,   92 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QStringList,    7,
    QMetaType::Void, QMetaType::QStringList,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::LongLong, QMetaType::Bool,   12,   13,   14,   15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   19,   20,
    QMetaType::Void, QMetaType::Int,   22,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   22,
    QMetaType::Void, QMetaType::QString,   27,
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 42,    5,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   44,   45,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   27,   49,
    QMetaType::Void, QMetaType::QString,   49,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   49,   52,
    QMetaType::Void, QMetaType::QString,   49,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   49,   27,
    QMetaType::Void, QMetaType::QString,   49,
    QMetaType::Void, QMetaType::QVariantList,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   62,   63,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   62,   63,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   66,   67,
    QMetaType::Void, QMetaType::QString,   69,

 // properties: name, type, flags
      70, QMetaType::Bool, 0x00015001, uint(21), 0,
      71, QMetaType::Bool, 0x00015001, uint(11), 0,
      21, QMetaType::Int, 0x00015001, uint(10), 0,
      72, QMetaType::QString, 0x00015001, uint(22), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject FileManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FileManager.offsetsAndSizes,
    qt_meta_data_FileManager,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_FileManager_t,
        // property 'isBusy'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'isUploading'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'uploadProgress'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'currentPath'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FileManager, std::true_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectionFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'directoryListed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'directoryListReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'clearFileList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addFileToList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'uploadStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'uploadFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'uploadProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'uploadProgressChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'isUploadingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'downloadProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'downloadCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'shareLinkGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'shareLinkCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'shareUrlGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fileOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'completed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'busyChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'currentPathChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'connectToPC'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'browseDirectory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'listDirectory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'uploadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'deleteFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'generateShareLink'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'generateShareUrl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'createShareLink'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'openFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'goBack'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goHome'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'downloadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'downloadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'downloadMultipleFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariantList &, std::false_type>,
        // method 'copyFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'moveFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'renameFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'createFolder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void FileManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectionFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->directoryListed((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 4: _t->directoryListReceived((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 5: _t->clearFileList(); break;
        case 6: _t->addFileToList((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5]))); break;
        case 7: _t->uploadStarted(); break;
        case 8: _t->uploadFinished((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->uploadProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->uploadProgressChanged(); break;
        case 11: _t->isUploadingChanged(); break;
        case 12: _t->downloadProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->downloadCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->shareLinkGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->shareLinkCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->shareUrlGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->fileOpened((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->completed(); break;
        case 19: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->busyChanged(); break;
        case 22: _t->currentPathChanged(); break;
        case 23: _t->onConnected(); break;
        case 24: _t->onReadyRead(); break;
        case 25: _t->onDisconnected(); break;
        case 26: _t->onError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 27: _t->connectToPC((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 28: _t->browseDirectory((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 29: _t->listDirectory((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 30: _t->uploadFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 31: _t->deleteFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->generateShareLink((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 33: _t->generateShareUrl((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 34: _t->createShareLink((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 35: _t->openFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 36: _t->goBack(); break;
        case 37: _t->goHome(); break;
        case 38: _t->disconnect(); break;
        case 39: _t->downloadFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 40: _t->downloadFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 41: _t->downloadMultipleFiles((*reinterpret_cast< std::add_pointer_t<QVariantList>>(_a[1]))); break;
        case 42: _t->copyFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 43: _t->moveFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 44: _t->renameFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 45: _t->createFolder((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 26:
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
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::disconnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::connectionFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QStringList & );
            if (_t _q_method = &FileManager::directoryListed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QStringList & );
            if (_t _q_method = &FileManager::directoryListReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::clearFileList; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & , const QString & , const QString & , qint64 , bool );
            if (_t _q_method = &FileManager::addFileToList; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::uploadStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(bool , const QString & );
            if (_t _q_method = &FileManager::uploadFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(int );
            if (_t _q_method = &FileManager::uploadProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::uploadProgressChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::isUploadingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(int );
            if (_t _q_method = &FileManager::downloadProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::downloadCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::shareLinkGenerated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::shareLinkCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::shareUrlGenerated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::fileOpened; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::completed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(const QString & );
            if (_t _q_method = &FileManager::errorOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 20;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::busyChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 21;
                return;
            }
        }
        {
            using _t = void (FileManager::*)();
            if (_t _q_method = &FileManager::currentPathChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 22;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<FileManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isBusy(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isUploading(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->uploadProgress(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->currentPath(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *FileManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FileManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void FileManager::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FileManager::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void FileManager::connectionFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FileManager::directoryListed(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void FileManager::directoryListReceived(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void FileManager::clearFileList()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void FileManager::addFileToList(const QString & _t1, const QString & _t2, const QString & _t3, qint64 _t4, bool _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void FileManager::uploadStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void FileManager::uploadFinished(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void FileManager::uploadProgress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void FileManager::uploadProgressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void FileManager::isUploadingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void FileManager::downloadProgress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void FileManager::downloadCompleted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void FileManager::shareLinkGenerated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void FileManager::shareLinkCreated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void FileManager::shareUrlGenerated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void FileManager::fileOpened(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void FileManager::completed()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void FileManager::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void FileManager::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void FileManager::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 21, nullptr);
}

// SIGNAL 22
void FileManager::currentPathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 22, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
