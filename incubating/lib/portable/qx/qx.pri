# qtx.pri
# 6/28/2011 jichi

DEFINES += WITH_LIB_QTX
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/qxglobal.h \
    $$PWD/qxdebug.h \
    $$PWD/qxdeclarativeview.h \
    $$PWD/qxdraggabledialog.h \
    $$PWD/qxdraggablemainwindow.h \
    $$PWD/qxdraggablequickview.h \
    $$PWD/qxdraggablewidget.h \
    $$PWD/qxtoolbutton.h \
    $$PWD/qxuiutil.h \
    $$PWD/qxwindowcontainer.h

SOURCES += \
    $$PWD/qxdeclarativeview.cc \
    $$PWD/qxdraggabledialog.cc \
    $$PWD/qxdraggablemainwindow.cc \
    $$PWD/qxdraggablequickview.cc \
    $$PWD/qxdraggablewidget.cc \
    $$PWD/qxtoolbutton.cc \
    $$PWD/qxwindowcontainer.cc

QT += core gui qml quick widgets

# EOF
