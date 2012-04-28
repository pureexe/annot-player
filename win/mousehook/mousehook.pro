# mousehook.pro
# 11/26/2011
include(../../config.pri)

## Sources

TEMPLATE = lib
TARGET  = mousehook
CONFIG  += dll

DEFINES += MOUSEHOOK_LIB
DEFINES += MOUSEHOOK_DLL_NAME=L\\\"$$TARGET\\\"

HEADERS = \
    mousehook_config.h \
    mousehook_p.h \
    mousehook.h

SOURCES = \
    mousehook.cc

OTHER_FILES = \
    mousehook.rc \
    mousehook.pri

!wince*: LIBS += -lshell32
RC_FILE += mousehook.rc

QT      += core gui
LIBS    += -luser32

#include($$ROOTDIR/win/qtwin/qtwin.pri)

# EOF
