/****************************************************************************
** Meta object code from reading C++ file 'usb_grabthreadworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../qt_pco_camera_usb/usb_grabthreadworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'usb_grabthreadworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_GrabThreadWorker_t {
    QByteArrayData data[5];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GrabThreadWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GrabThreadWorker_t qt_meta_stringdata_GrabThreadWorker = {
    {
QT_MOC_LITERAL(0, 0, 16), // "GrabThreadWorker"
QT_MOC_LITERAL(1, 17, 17), // "finishedAllSignal"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 15), // "finishedPicture"
QT_MOC_LITERAL(4, 52, 13) // "recorderError"

    },
    "GrabThreadWorker\0finishedAllSignal\0\0"
    "finishedPicture\0recorderError"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GrabThreadWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       3,    1,   32,    2, 0x06 /* Public */,
       4,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void GrabThreadWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GrabThreadWorker *_t = static_cast<GrabThreadWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->finishedAllSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->finishedPicture((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->recorderError((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GrabThreadWorker::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GrabThreadWorker::finishedAllSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GrabThreadWorker::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GrabThreadWorker::finishedPicture)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (GrabThreadWorker::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GrabThreadWorker::recorderError)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject GrabThreadWorker::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_GrabThreadWorker.data,
      qt_meta_data_GrabThreadWorker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *GrabThreadWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GrabThreadWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_GrabThreadWorker.stringdata0))
        return static_cast<void*>(const_cast< GrabThreadWorker*>(this));
    return QThread::qt_metacast(_clname);
}

int GrabThreadWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void GrabThreadWorker::finishedAllSignal(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GrabThreadWorker::finishedPicture(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GrabThreadWorker::recorderError(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
