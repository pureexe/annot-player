# mecabparser.pri
# 9/3/2012

DEFINES += WITH_MODULE_MECABPARSER

DEPENDPATH += $$PWD

HEADERS += $$PWD/mecabparser.h
SOURCES += $$PWD/mecabparser.cc

LIBS += -lmecab
QT += core gui

# EOF
