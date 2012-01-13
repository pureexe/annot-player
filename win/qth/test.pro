# test.pro
# 10/15/2011

include(../../config.pri)
include($$ROOTDIR/win/qtwin/qtwin.pri)

DEPENDPATH  += $$PWD
INCLUDEPATH += $$PWD

## Libraries

#DEFINES += _ITH_DEBUG_MEMORY

QT      += core gui

#WDK_HOME    = c:/winddk
#LIBS        += -L$$WDK_HOME/lib
# override folder must come before winddk/inc/api
#INCLUDEPATH += $$PWD/override/wdk/vc10
#INCLUDEPATH += $$WDK_HOME/include/api
#INCLUDEPATH += $$WDK_HOME/include/crt
#INCLUDEPATH += $$WDK_HOME/include/ddk

#ITH_HOME    = c:/dev/ith
#INCLUDEPATH += $$ITH_HOME/include
#LIBS        += -L$$ITH_HOME/lib
#LIBS        += -lITH_DLL #-lITH_SYS

# WINDDK
LIBS    += -lntdll
# WINSDK
LIBS    += -luser32

#LIBS    += -lmsvcrtd
#LIBS    += -lmsvcrt
#QMAKE_LFLAGS += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcrtd.lib
DEFINES += _CRT_SECURE_NO_WARNING _CRT_NON_CONFORMING_SWPRINTFS

## Sources

TEMPLATE = app
TARGET =

HEADERS += \
    $$PWD/ith.h \
    $$PWD/qth.h \
    $$PWD/ith/AVL.h \
    $$PWD/ith/common.h \
    $$PWD/ith/hookman.h \
    $$PWD/ith/inject.h \
    $$PWD/ith/language.h \
    $$PWD/ith/main.h \
    $$PWD/ith/main_template.h \
    $$PWD/ith/pipe.h \
    $$PWD/ith/protocol.h \
    $$PWD/ith/query.h \
    $$PWD/ith/sys.h \
    $$PWD/ith/text.h

SOURCES += \
    $$PWD/ith.cc \
    $$PWD/qth.cc \
    $$PWD/test.cc \
    $$PWD/ith/inject.cc \
    $$PWD/ith/language.cc \
    $$PWD/ith/pipe.cc \
    $$PWD/ith/query.cc \
    $$PWD/ith/utility.cc \
    $$PWD/ith/sys.cc

#LIBS    += -lshell32
#RC_FILE += ITH.rc

OTHER_FILES += \
    ChangeLog \
    TODO

# EOF
