/****************************************************************************
** Meta object code from reading C++ file 'resulttubesimulation.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "panel/resulttubesimulation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'resulttubesimulation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ResultTubeSimulation_t {
    QByteArrayData data[10];
    char stringdata0[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ResultTubeSimulation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ResultTubeSimulation_t qt_meta_stringdata_ResultTubeSimulation = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ResultTubeSimulation"
QT_MOC_LITERAL(1, 21, 16), // "triggerUpToggled"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 20), // "onRefreshFileClicked"
QT_MOC_LITERAL(4, 60, 17), // "onChargeDataReady"
QT_MOC_LITERAL(5, 78, 20), // "onProjectInfoChanged"
QT_MOC_LITERAL(6, 99, 11), // "ProjectInfo"
QT_MOC_LITERAL(7, 111, 4), // "info"
QT_MOC_LITERAL(8, 116, 13), // "scriptChanged"
QT_MOC_LITERAL(9, 130, 8) // "filePath"

    },
    "ResultTubeSimulation\0triggerUpToggled\0"
    "\0onRefreshFileClicked\0onChargeDataReady\0"
    "onProjectInfoChanged\0ProjectInfo\0info\0"
    "scriptChanged\0filePath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ResultTubeSimulation[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    1,   42,    2, 0x0a /* Public */,
       8,    1,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    9,

       0        // eod
};

void ResultTubeSimulation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ResultTubeSimulation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->triggerUpToggled(); break;
        case 1: _t->onRefreshFileClicked(); break;
        case 2: _t->onChargeDataReady(); break;
        case 3: _t->onProjectInfoChanged((*reinterpret_cast< const ProjectInfo(*)>(_a[1]))); break;
        case 4: _t->scriptChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ResultTubeSimulation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ResultTubeSimulation::triggerUpToggled)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ResultTubeSimulation::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ResultTubeSimulation.data,
    qt_meta_data_ResultTubeSimulation,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ResultTubeSimulation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ResultTubeSimulation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ResultTubeSimulation.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ResultTubeSimulation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ResultTubeSimulation::triggerUpToggled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
