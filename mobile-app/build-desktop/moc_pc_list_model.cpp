/****************************************************************************
** Meta object code from reading C++ file 'pc_list_model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/pc_list_model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pc_list_model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PCListModel_t {
    QByteArrayData data[15];
    char stringdata0[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PCListModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PCListModel_t qt_meta_stringdata_PCListModel = {
    {
QT_MOC_LITERAL(0, 0, 11), // "PCListModel"
QT_MOC_LITERAL(1, 12, 12), // "countChanged"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 5), // "addPC"
QT_MOC_LITERAL(4, 32, 2), // "id"
QT_MOC_LITERAL(5, 35, 4), // "name"
QT_MOC_LITERAL(6, 40, 9), // "ipAddress"
QT_MOC_LITERAL(7, 50, 6), // "status"
QT_MOC_LITERAL(8, 57, 6), // "osType"
QT_MOC_LITERAL(9, 64, 8), // "updatePC"
QT_MOC_LITERAL(10, 73, 8), // "removePC"
QT_MOC_LITERAL(11, 82, 5), // "clear"
QT_MOC_LITERAL(12, 88, 3), // "get"
QT_MOC_LITERAL(13, 92, 5), // "index"
QT_MOC_LITERAL(14, 98, 5) // "count"

    },
    "PCListModel\0countChanged\0\0addPC\0id\0"
    "name\0ipAddress\0status\0osType\0updatePC\0"
    "removePC\0clear\0get\0index\0count"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PCListModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       1,   68, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       3,    5,   45,    2, 0x02 /* Public */,
       9,    2,   56,    2, 0x02 /* Public */,
      10,    1,   61,    2, 0x02 /* Public */,
      11,    0,   64,    2, 0x02 /* Public */,
      12,    1,   65,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,    4,    5,    6,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    4,    7,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::QVariantMap, QMetaType::Int,   13,

 // properties: name, type, flags
      14, QMetaType::Int, 0x00495001,

 // properties: notify_signal_id
       0,

       0        // eod
};

void PCListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PCListModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->countChanged(); break;
        case 1: _t->addPC((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5]))); break;
        case 2: _t->updatePC((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->removePC((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->clear(); break;
        case 5: { QVariantMap _r = _t->get((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PCListModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PCListModel::countChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<PCListModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->count(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject PCListModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_PCListModel.data,
    qt_meta_data_PCListModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PCListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PCListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PCListModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int PCListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
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
void PCListModel::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
