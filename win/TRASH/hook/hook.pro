# hook.pro
# 6/30/2011
include(../../config.pri)

QT      += core gui
LIBS    += -luser32

## Sources

TEMPLATE = lib
TARGET  = hook
CONFIG  += dll

DEFINES += HOOK_LIB
DEFINES += USE_MOUSE_HOOK
DEFINES += HOOK_DLL_NAME=L\\\"$$TARGET\\\"

HEADERS = \
    hook_config.h \
    hook.h

SOURCES = \
    hook.cc

OTHER_FILES = \
    hook.rc \
    hook.pri

!wince*: LIBS += -lshell32
RC_FILE += hook.rc

include($$ROOTDIR/win/qtwin/qtwin.pri)

# EOF
