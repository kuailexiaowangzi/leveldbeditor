QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    add.cpp \
    database.cpp \
    delete.cpp \
    edit.cpp \
    main.cpp \
    mainwindow.cpp \
    menushadow.cpp \
    new.cpp \
    open.cpp \
    otherborder.cpp \
    position.cpp \
    tooltip.cpp \
    traversalposition.cpp \
    widget.cpp

HEADERS += \
    about.h \
    add.h \
    database.h \
    delete.h \
    edit.h \
    leveldb/c.h \
    leveldb/cache.h \
    leveldb/comparator.h \
    leveldb/db.h \
    leveldb/dumpfile.h \
    leveldb/env.h \
    leveldb/export.h \
    leveldb/filter_policy.h \
    leveldb/iterator.h \
    leveldb/options.h \
    leveldb/slice.h \
    leveldb/status.h \
    leveldb/table.h \
    leveldb/table_builder.h \
    leveldb/write_batch.h \
    mainwindow.h \
    menushadow.h \
    new.h \
    open.h \
    otherborder.h \
    position.h \
    tooltip.h \
    traversalposition.h \
    widget.h

FORMS += \
    about.ui \
    add.ui \
    delete.ui \
    edit.ui \
    mainwindow.ui \
    menushadow.ui \
    new.ui \
    open.ui \
    otherborder.ui \
    tooltip.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    main.qrc

TRANSLATIONS += \
    language/zh_cn.ts

win {
    LIBS += -lUser32
}

unix|win32: LIBS += \
    -L$$PWD/lib/ -lleveldb

INCLUDEPATH += \
    $$PWD/leveldb
