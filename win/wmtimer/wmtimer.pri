# qtwin.pri
# 7/20/2011

DEFINES += WITH_WIN_QTWIN

INCLUDEPATH     += $$PWD
DEPENDPATH      += $$PWD

HEADERS += $$PWD/wmtimer.h 
SOURCES += $$PWD/wmtimer.cc

QT      += core gui
LIBS    += -lkernel32 -luser32 

# EOF
