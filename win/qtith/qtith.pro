# qtith.pro
# 10/13/2011

include(../../config.pri)

## Libraries

DEPENDPATH += .
INCLUDEPATH += .

#DEFINES += _ITH_DEBUG_MEMORY

#QT      += core

WDK_HOME    = c:/winddk
LIBS        += -L$$WDK_HOME/lib
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
#LIBS    += -lkernel32
LIBS    += -luser32 -lgdi32 -lcomdlg32 -lshell32 -lcomctl32

#LIBS    += -lmsvcrtd
LIBS    += -lmsvcrt
QMAKE_LFLAGS += /NODEFAULTLIB:msvcrt.lib

## Sources

TEMPLATE = app
TARGET =

HEADERS += \
    AVL.h \
    cmdq.h \
    common.h \
    hookman.h \
    language.h \
    main.h \
    main_template.h \
    ntdll.h \
    profile.h \
    qtith.h \
    resource.h \
    sys.h \
    text.h \
    version.h \
    window.h

SOURCES += \
    command.cpp \
    disasm.cpp \
    inject.cpp \
    language.cpp \
    main.cpp \
    pipe.cpp \
    profile.cpp \
    qtith.cc \
    SYS.cpp \
    utility.cpp \
    window.cpp

RC_FILE += ITH.rc

OTHER_FILES += \
    ITH.rc \
    ChangeLog \
    TODO

# EOF
