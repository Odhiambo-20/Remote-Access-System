/****************************************************************************
** Meta object code from reading C++ file 'filemanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/filemanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filemanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileManager_t {
    QByteArrayData data[20];
    char stringdata0[237];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileManager_t qt_meta_stringdata_FileManager = {
    {
QT_MOC_LITERAL(0, 0, 11), // "FileManager"
QT_MOC_LITERAL(1, 12, 18), // "currentPathChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 22), // "fileOperationCompleted"
QT_MOC_LITERAL(4, 55, 7), // "success"
QT_MOC_LITERAL(5, 63, 7), // "message"
QT_MOC_LITERAL(6, 71, 16), // "downloadProgress"
QT_MOC_LITERAL(7, 88, 13), // "bytesReceived"
QT_MOC_LITERAL(8, 102, 10), // "bytesTotal"
QT_MOC_LITERAL(9, 113, 14), // "uploadProgress"
QT_MOC_LITERAL(10, 128, 9), // "bytesSent"
QT_MOC_LITERAL(11, 138, 9), // "listFiles"
QT_MOC_LITERAL(12, 148, 4), // "path"
QT_MOC_LITERAL(13, 153, 12), // "downloadFile"
QT_MOC_LITERAL(14, 166, 10), // "remotePath"
QT_MOC_LITERAL(15, 177, 9), // "localPath"
QT_MOC_LITERAL(16, 187, 10), // "uploadFile"
QT_MOC_LITERAL(17, 198, 10), // "deleteFile"
QT_MOC_LITERAL(18, 209, 15), // "createDirectory"
QT_MOC_LITERAL(19, 225, 11) // "currentPath"

    },
    "FileManager\0currentPathChanged\0\0"
    "fileOperationCompleted\0success\0message\0"
    "downloadProgress\0bytesReceived\0"
    "bytesTotal\0uploadProgress\0bytesSent\0"
    "listFiles\0path\0downloadFile\0remotePath\0"
    "localPath\0uploadFile\0deleteFile\0"
    "createDirectory\0currentPath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       1,   94, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    2,   60,    2, 0x06 /* Public */,
       6,    2,   65,    2, 0x06 /* Public */,
       9,    2,   70,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
      11,    1,   75,    2, 0x02 /* Public */,
      13,    2,   78,    2, 0x02 /* Public */,
      16,    2,   83,    2, 0x02 /* Public */,
      17,    1,   88,    2, 0x02 /* Public */,
      18,    1,   91,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    4,    5,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    7,    8,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,   10,    8,

 // methods: parameters
    QMetaType::QVariantList, QMetaType::QString,   12,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,   14,   15,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,   15,   14,
    QMetaType::Bool, QMetaType::QString,   12,
    QMetaType::Bool, QMetaType::QString,   12,

 // properties: name, type, flags
      19, QMetaType::QString, 0x00495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

void FileManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentPathChanged(); break;
        case 1: _t->fileOperationCompleted((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->downloadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 3: _t->uploadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 4: { QVariantList _r = _t->listFiles((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 5: { bool _r = _t->downloadFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->uploadFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { bool _r = _t->deleteFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { bool _r = _t->createDirectory((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManager::currentPathChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManager::fileOperationCompleted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManager::downloadProgress)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FileManager::*)(qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManager::uploadProgress)) {
                *result = 3;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<FileManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->currentPath(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<FileManager *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setCurrentPath(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject FileManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_FileManager.data,
    qt_meta_data_FileManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


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
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
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
void FileManager::currentPathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FileManager::fileOperationCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FileManager::downloadProgress(qint64 _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FileManager::uploadProgress(qint64 _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
