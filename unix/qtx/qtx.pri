# qtx.pri
# 11/25/2011
INCLUDEPATH += $$PWD

DEFINES += WITH_UNIX_QTX

HEADERS += $$PWD/qtx.h

SOURCES += $$PWD/qtx.cc

QT      += core gui
LIBS    += -lX11 -lXfixes

# EOF
