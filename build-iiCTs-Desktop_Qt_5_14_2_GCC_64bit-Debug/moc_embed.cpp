/****************************************************************************
** Meta object code from reading C++ file 'embed.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../iiCTs/include/widget/embed.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'embed.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_EmbedWidget_t {
    QByteArrayData data[13];
    char stringdata0[194];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EmbedWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EmbedWidget_t qt_meta_stringdata_EmbedWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "EmbedWidget"
QT_MOC_LITERAL(1, 12, 13), // "geant4Started"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 14), // "geant4Finished"
QT_MOC_LITERAL(4, 42, 8), // "exitCode"
QT_MOC_LITERAL(5, 51, 14), // "geant4Embedded"
QT_MOC_LITERAL(6, 66, 13), // "errorOccurred"
QT_MOC_LITERAL(7, 80, 12), // "errorMessage"
QT_MOC_LITERAL(8, 93, 22), // "onGeant4StandardOutput"
QT_MOC_LITERAL(9, 116, 21), // "onGeant4StandardError"
QT_MOC_LITERAL(10, 138, 23), // "onGeant4ProcessFinished"
QT_MOC_LITERAL(11, 162, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(12, 183, 10) // "exitStatus"

    },
    "EmbedWidget\0geant4Started\0\0geant4Finished\0"
    "exitCode\0geant4Embedded\0errorOccurred\0"
    "errorMessage\0onGeant4StandardOutput\0"
    "onGeant4StandardError\0onGeant4ProcessFinished\0"
    "QProcess::ExitStatus\0exitStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EmbedWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    1,   50,    2, 0x06 /* Public */,
       5,    0,   53,    2, 0x06 /* Public */,
       6,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   57,    2, 0x08 /* Private */,
       9,    0,   58,    2, 0x08 /* Private */,
      10,    2,   59,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 11,    4,   12,

       0        // eod
};

void EmbedWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EmbedWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->geant4Started(); break;
        case 1: _t->geant4Finished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->geant4Embedded(); break;
        case 3: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onGeant4StandardOutput(); break;
        case 5: _t->onGeant4StandardError(); break;
        case 6: _t->onGeant4ProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EmbedWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbedWidget::geant4Started)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (EmbedWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbedWidget::geant4Finished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (EmbedWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbedWidget::geant4Embedded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (EmbedWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbedWidget::errorOccurred)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject EmbedWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_EmbedWidget.data,
    qt_meta_data_EmbedWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *EmbedWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EmbedWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EmbedWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int EmbedWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void EmbedWidget::geant4Started()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void EmbedWidget::geant4Finished(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EmbedWidget::geant4Embedded()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void EmbedWidget::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
