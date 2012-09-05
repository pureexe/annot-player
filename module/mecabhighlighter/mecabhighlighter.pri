# mecabhighlighter.pri
# 9/3/2012

DEFINES += WITH_MODULE_MECABHIGHLIGHTER

DEPENDPATH += $$PWD

HEADERS += $$PWD/mecabhighlighter.h
SOURCES += $$PWD/mecabhighlighter.cc

LIBS += -lmecab
QT += core gui

# EOF
