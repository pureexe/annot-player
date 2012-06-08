# qtwinnt.pri
# 6/6/2012

DEFINES += WITH_WIN_QTWINNT

INCLUDEPATH     += $$PWD/..
INCLUDEPATH     += $$PWD
DEPENDPATH      += $$PWD

HEADERS += \
    $$PWD/ntdll.h \
    $$PWD/qtwinnt.h

SOURCES += \
    $$PWD/qtwinnt.cc

QT      += core
LIBS    += -lkernel32 -luser32 -lntdll

# EOF
