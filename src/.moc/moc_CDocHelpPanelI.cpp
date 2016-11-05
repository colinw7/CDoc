/****************************************************************************
** Meta object code from reading C++ file 'CDocHelpPanelI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CDocHelpPanelI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDocHelpPanelI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CDocHelpCanvas_t {
    QByteArrayData data[3];
    char stringdata0[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDocHelpCanvas_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDocHelpCanvas_t qt_meta_stringdata_CDocHelpCanvas = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CDocHelpCanvas"
QT_MOC_LITERAL(1, 15, 12), // "scrolledSlot"
QT_MOC_LITERAL(2, 28, 0) // ""

    },
    "CDocHelpCanvas\0scrolledSlot\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDocHelpCanvas[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CDocHelpCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CDocHelpCanvas *_t = static_cast<CDocHelpCanvas *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->scrolledSlot(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CDocHelpCanvas::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CDocHelpCanvas.data,
      qt_meta_data_CDocHelpCanvas,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CDocHelpCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDocHelpCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CDocHelpCanvas.stringdata0))
        return static_cast<void*>(const_cast< CDocHelpCanvas*>(this));
    return QWidget::qt_metacast(_clname);
}

int CDocHelpCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_CDHelpPanel_t {
    QByteArrayData data[15];
    char stringdata0[177];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDHelpPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDHelpPanel_t qt_meta_stringdata_CDHelpPanel = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CDHelpPanel"
QT_MOC_LITERAL(1, 12, 17), // "selectSectionSlot"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "viewRefsSlot"
QT_MOC_LITERAL(4, 44, 11), // "listAllSlot"
QT_MOC_LITERAL(5, 56, 10), // "searchSlot"
QT_MOC_LITERAL(6, 67, 8), // "quitSlot"
QT_MOC_LITERAL(7, 76, 13), // "firstPageSlot"
QT_MOC_LITERAL(8, 90, 12), // "lastPageSlot"
QT_MOC_LITERAL(9, 103, 12), // "nextPageSlot"
QT_MOC_LITERAL(10, 116, 12), // "prevPageSlot"
QT_MOC_LITERAL(11, 129, 14), // "selectPageSlot"
QT_MOC_LITERAL(12, 144, 8), // "helpSlot"
QT_MOC_LITERAL(13, 153, 13), // "clipBoardSlot"
QT_MOC_LITERAL(14, 167, 9) // "printSlot"

    },
    "CDHelpPanel\0selectSectionSlot\0\0"
    "viewRefsSlot\0listAllSlot\0searchSlot\0"
    "quitSlot\0firstPageSlot\0lastPageSlot\0"
    "nextPageSlot\0prevPageSlot\0selectPageSlot\0"
    "helpSlot\0clipBoardSlot\0printSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDHelpPanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    0,   82,    2, 0x08 /* Private */,
       6,    0,   83,    2, 0x08 /* Private */,
       7,    0,   84,    2, 0x08 /* Private */,
       8,    0,   85,    2, 0x08 /* Private */,
       9,    0,   86,    2, 0x08 /* Private */,
      10,    0,   87,    2, 0x08 /* Private */,
      11,    0,   88,    2, 0x08 /* Private */,
      12,    0,   89,    2, 0x08 /* Private */,
      13,    0,   90,    2, 0x08 /* Private */,
      14,    0,   91,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CDHelpPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CDHelpPanel *_t = static_cast<CDHelpPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectSectionSlot(); break;
        case 1: _t->viewRefsSlot(); break;
        case 2: _t->listAllSlot(); break;
        case 3: _t->searchSlot(); break;
        case 4: _t->quitSlot(); break;
        case 5: _t->firstPageSlot(); break;
        case 6: _t->lastPageSlot(); break;
        case 7: _t->nextPageSlot(); break;
        case 8: _t->prevPageSlot(); break;
        case 9: _t->selectPageSlot(); break;
        case 10: _t->helpSlot(); break;
        case 11: _t->clipBoardSlot(); break;
        case 12: _t->printSlot(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CDHelpPanel::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CDHelpPanel.data,
      qt_meta_data_CDHelpPanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CDHelpPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDHelpPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CDHelpPanel.stringdata0))
        return static_cast<void*>(const_cast< CDHelpPanel*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CDHelpPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
