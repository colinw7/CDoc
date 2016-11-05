/****************************************************************************
** Meta object code from reading C++ file 'CDocHelpSearchPanelI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CDocHelpSearchPanelI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDocHelpSearchPanelI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CDHelpSearchPanel_t {
    QByteArrayData data[8];
    char stringdata0[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDHelpSearchPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDHelpSearchPanel_t qt_meta_stringdata_CDHelpSearchPanel = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CDHelpSearchPanel"
QT_MOC_LITERAL(1, 18, 13), // "findFirstSlot"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "findLastSlot"
QT_MOC_LITERAL(4, 46, 12), // "findPrevSlot"
QT_MOC_LITERAL(5, 59, 12), // "findNextSlot"
QT_MOC_LITERAL(6, 72, 10), // "cancelSlot"
QT_MOC_LITERAL(7, 83, 8) // "helpSlot"

    },
    "CDHelpSearchPanel\0findFirstSlot\0\0"
    "findLastSlot\0findPrevSlot\0findNextSlot\0"
    "cancelSlot\0helpSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDHelpSearchPanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x08 /* Private */,
       7,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CDHelpSearchPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CDHelpSearchPanel *_t = static_cast<CDHelpSearchPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->findFirstSlot(); break;
        case 1: _t->findLastSlot(); break;
        case 2: _t->findPrevSlot(); break;
        case 3: _t->findNextSlot(); break;
        case 4: _t->cancelSlot(); break;
        case 5: _t->helpSlot(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CDHelpSearchPanel::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CDHelpSearchPanel.data,
      qt_meta_data_CDHelpSearchPanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CDHelpSearchPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDHelpSearchPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CDHelpSearchPanel.stringdata0))
        return static_cast<void*>(const_cast< CDHelpSearchPanel*>(this));
    return QDialog::qt_metacast(_clname);
}

int CDHelpSearchPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
    return _id;
}
QT_END_MOC_NAMESPACE
