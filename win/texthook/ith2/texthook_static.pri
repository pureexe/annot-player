# texthook_static.pri
# 10/26/2011

include(../../config.pri)

DEFINES += WITH_WIN_TEXTHOOK

INCLUDEPATH += \
    $$PWD
DEPENDPATH += \
    $$PWD \
    $$PWD/ith

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

DEFINES += TEXTHOOK_STATICLIB

HEADERS += \
    ith.h \
    texthook_config.h \
    texthook.h \
    ith/AVL.h \
    ith/common.h \
    ith/hookman.h \
    ith/inject.h \
    ith/language.h \
    ith/main.h \
    ith/main_template.h \
    ith/pipe.h \
    ith/query.h \
    ith/sys.h \
    ith/text.h

SOURCES += \
    ith.cc \
    texthook.cc \
    ith/inject.cc \
    ith/language.cc \
    ith/pipe.cc \
    ith/query.cc \
    ith/utility.cc \
    ith/sys.cc

# EOF
