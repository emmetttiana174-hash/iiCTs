/****************************************************************************
** Meta object code from reading C++ file 'modelctscansimulation.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "panel/modelctscansimulation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelctscansimulation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ModelCTScanSimulation_t {
    QByteArrayData data[6];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModelCTScanSimulation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModelCTScanSimulation_t qt_meta_stringdata_ModelCTScanSimulation = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ModelCTScanSimulation"
QT_MOC_LITERAL(1, 22, 20), // "onProjectInfoChanged"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 11), // "ProjectInfo"
QT_MOC_LITERAL(4, 56, 4), // "info"
QT_MOC_LITERAL(5, 61, 16) // "onRefreshClicked"

    },
    "ModelCTScanSimulation\0onProjectInfoChanged\0"
    "\0ProjectInfo\0info\0onRefreshClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModelCTScanSimulation[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       5,    0,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

       0        // eod
};

void ModelCTScanSimulation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModelCTScanSimulation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onProjectInfoChanged((*reinterpret_cast< const ProjectInfo(*)>(_a[1]))); break;
        case 1: _t->onRefreshClicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ModelCTScanSimulation::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ModelCTScanSimulation.data,
    qt_meta_data_ModelCTScanSimulation,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ModelCTScanSimulation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModelCTScanSimulation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ModelCTScanSimulation.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ModelCTScanSimulation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
