# ipc.pri
# 4/9/2012

DEFINES += WITH_MODULE_IPC

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/metacallevent.h \
    $$PWD/metacallfilter.h \
    $$PWD/metacallpropagator.h
SOURCES += \
    $$PWD/metacallfilter.cc \
    $$PWD/metacallpropagator.cc

QT += core network

# EOF
