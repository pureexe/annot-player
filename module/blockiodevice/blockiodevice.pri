# blockiodevice.pri
# 12/3/2011

include(../../config.pri)

DEFINES += WITH_MODULE_BLOCKIODEVICE

DEPENDPATH += $$PWD

win32: INCLUDEPATH += $$WDK_HOME/inc/api

HEADERS += $$PWD/blockiodevice.h
SOURCES += $$PWD/blockiodevice.cc

QT      += core

# EOF
