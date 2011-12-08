# picker.pro
# 10/30/2011
include(../../config.pri)

QT      += core gui
LIBS    += -luser32

## Sources

TEMPLATE = lib
TARGET  = picker
CONFIG  += dll

DEFINES += PICKER_LIB
DEFINES += PICKER_DLL_NAME=L\\\"$$TARGET\\\"

HEADERS = \
    picker_config.h \
    picker.h

SOURCES = \
    picker.cc

OTHER_FILES = \
    picker.rc \
    picker.pri

!wince*: LIBS += -lshell32
RC_FILE += picker.rc

#include($$ROOTDIR/win/qtwin/qtwin.pri)

# EOF
