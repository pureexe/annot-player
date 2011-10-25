# qth.pro
# 10/13/2011

include(../../config.pri)
include($$ROOTDIR/win/qtwin/qtwin.pri)

DEPENDPATH  += .
INCLUDEPATH += .

## Libraries

#DEFINES += _ITH_DEBUG_MEMORY

QT      += core

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

TEMPLATE = lib
TARGET  = qth
CONFIG  += dll

DEFINES += QTH_LIB

HEADERS += \
    ith.h \
    qth_config.h \
    qth.h \
    ith/AVL.h \
    ith/cmdq.h \
    ith/common.h \
    ith/hookman.h \
    ith/language.h \
    ith/main.h \
    ith/main_template.h \
    ith/resource.h \
    ith/sys.h \
    ith/text.h

SOURCES += \
    ith.cc \
    qth.cc \
    ith/command.cc \
    ith/inject.cc \
    ith/language.cc \
    ith/pipe.cc \
    ith/utility.cc \
    ith/sys.cc

!wince*: LIBS += -lshell32
RC_FILE += qth.rc

OTHER_FILES += \
    qth.pri \
    qth.rc \
    test.cc \
    test.pro

# EOF
