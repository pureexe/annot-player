# qtwin.pri
# 7/20/2011

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/ntdll.h \
    $$PWD/qtwin.h \
    $$PWD/qtwinapi.h

SOURCES += \
    $$PWD/qtwin.cc \
    $$PWD/qtwinapi.cc

QT      += core gui
LIBS    += -lkernel32 -luser32 -lpsapi -lole32 #-lwinmm

# EOF
