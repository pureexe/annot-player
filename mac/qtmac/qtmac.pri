# qtmac.pri
# 11/11/2011

DEFINES += WITH_MAC_QTMAC

INCLUDEPATH     += $$PWD/..
DEPENDPATH      += $$PWD

HEADERS += $$PWD/qtmac.h
SOURCES += $$PWD/qtmac.cc

QT      += core gui

#LIBS    += -lIOKit
#DEFINES += WITH_IOKIT

# EOF
