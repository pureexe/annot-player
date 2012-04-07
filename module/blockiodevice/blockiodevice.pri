# blockiodevice.pri
# 12/3/2011

include(../../config.pri)

DEFINES += WITH_MODULE_BLOCKIODEVICE

win32: INCLUDEPATH += $$WDK_HOME/include/api

INCLUDEPATH += $$PWD
HEADERS += $$PWD/blockiodevice.h
SOURCES += $$PWD/blockiodevice.cc

QT      += core

# EOF
