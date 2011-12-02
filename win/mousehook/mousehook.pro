# mousehook.pro
# 11/26/2011
include(../../config.pri)

QT      += core gui
LIBS    += -luser32

## Sources

TEMPLATE = lib
TARGET  = mousehook
CONFIG  += dll

DEFINES += MOUSEHOOK_LIB
DEFINES += MOUSEHOOK_DLL_NAME=L\\\"$$TARGET\\\"

HEADERS = \
    mousehook_config.h \
    mousehook.h

SOURCES = \
    mousehook.cc

OTHER_FILES = \
    mousehook.rc \
    mousehook.pri

!wince*: LIBS += -lshell32
RC_FILE += mousehook.rc

#include($$ROOTDIR/win/qtwin/qtwin.pri)

# EOF
