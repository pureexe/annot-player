# texthook.pro
# 10/13/2011

include(../../config.pri)
include($$ROOTDIR/win/qtwin/qtwin.pri)
include($$ROOTDIR/win/wmtimer/wmtimer.pri)

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

LIBS    += -lihf

#LIBS    += -lmsvcrtd
#LIBS    += -lmsvcrt
#QMAKE_LFLAGS += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcrtd.lib
DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_NON_CONFORMING_SWPRINTFS


# Tell IHF not to override new operators, see: ith/mem.h
DEFINES += DEFAULT_MM

## Sources

TEMPLATE = lib
TARGET  = texthook
CONFIG  += dll

DEFINES += TEXTHOOK_LIB

HEADERS += \
    ith.h \
    texthook_config.h \
    texthook.h \
    textthread_p.h

SOURCES += \
    ith.cc \
    texthook.cc \
    textthread_p.cc 

!wince*: LIBS += -lshell32
RC_FILE += texthook.rc

OTHER_FILES += \
    texthooik.pri \
    texthook.rc \
    test.cc \
    test.pro

# EOF
