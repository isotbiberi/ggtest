/****************************************************************************
** Meta object code from reading C++ file 'camerasetup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../pco_common/qt_pco_camera/camerasetup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camerasetup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CameraSetup_t {
    QByteArrayData data[14];
    char stringdata0[241];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CameraSetup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CameraSetup_t qt_meta_stringdata_CameraSetup = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CameraSetup"
QT_MOC_LITERAL(1, 12, 24), // "on_ROIx0_editingFinished"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 24), // "on_ROIx1_editingFinished"
QT_MOC_LITERAL(4, 63, 24), // "on_ROIy0_editingFinished"
QT_MOC_LITERAL(5, 88, 24), // "on_ROIy1_editingFinished"
QT_MOC_LITERAL(6, 113, 11), // "setROIx1sym"
QT_MOC_LITERAL(7, 125, 5), // "value"
QT_MOC_LITERAL(8, 131, 11), // "setROIy1sym"
QT_MOC_LITERAL(9, 143, 23), // "on_ROImaxbutton_clicked"
QT_MOC_LITERAL(10, 167, 22), // "on_ApplyButton_clicked"
QT_MOC_LITERAL(11, 190, 17), // "SetCheckboxToIdle"
QT_MOC_LITERAL(12, 208, 26), // "on_tabWidget_tabBarClicked"
QT_MOC_LITERAL(13, 235, 5) // "index"

    },
    "CameraSetup\0on_ROIx0_editingFinished\0"
    "\0on_ROIx1_editingFinished\0"
    "on_ROIy0_editingFinished\0"
    "on_ROIy1_editingFinished\0setROIx1sym\0"
    "value\0setROIy1sym\0on_ROImaxbutton_clicked\0"
    "on_ApplyButton_clicked\0SetCheckboxToIdle\0"
    "on_tabWidget_tabBarClicked\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CameraSetup[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    1,   68,    2, 0x08 /* Private */,
       8,    1,   71,    2, 0x08 /* Private */,
       9,    0,   74,    2, 0x08 /* Private */,
      10,    0,   75,    2, 0x08 /* Private */,
      11,    0,   76,    2, 0x08 /* Private */,
      12,    1,   77,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   13,

       0        // eod
};

void CameraSetup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CameraSetup *_t = static_cast<CameraSetup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_ROIx0_editingFinished(); break;
        case 1: _t->on_ROIx1_editingFinished(); break;
        case 2: _t->on_ROIy0_editingFinished(); break;
        case 3: _t->on_ROIy1_editingFinished(); break;
        case 4: _t->setROIx1sym((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setROIy1sym((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_ROImaxbutton_clicked(); break;
        case 7: _t->on_ApplyButton_clicked(); break;
        case 8: _t->SetCheckboxToIdle(); break;
        case 9: _t->on_tabWidget_tabBarClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CameraSetup::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CameraSetup.data,
      qt_meta_data_CameraSetup,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CameraSetup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CameraSetup::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CameraSetup.stringdata0))
        return static_cast<void*>(const_cast< CameraSetup*>(this));
    if (!strcmp(_clname, "Ui::CameraSetup"))
        return static_cast< Ui::CameraSetup*>(const_cast< CameraSetup*>(this));
    return QDialog::qt_metacast(_clname);
}

int CameraSetup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
