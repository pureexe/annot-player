# qtx11.pri
# 11/25/2011

DEFINES += WITH_UNIX_QTX11

DEPENDPATH      += $$PWD

HEADERS += $$PWD/qtx11.h
SOURCES += $$PWD/qtx11.cc

QT      += core gui
LIBS    += -lX11 -lXfixes

# EOF
