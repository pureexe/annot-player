# qtwin.pri
# 7/20/2011 jichi

DEFINES += WITH_WIN_QTWIN

INCLUDEPATH  += $$PWD
DEPENDPATH   += $$PWD

HEADERS += $$PWD/qtwin.h
SOURCES += $$PWD/qtwin.cc

QT      += core gui
LIBS    += -lkernel32 -luser32 -lpsapi -lshell32 #-lwinmm

# EOF
