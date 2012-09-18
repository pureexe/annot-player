# edrdg.pri
# 9/11/2012

DEFINES += WITH_LIB_EDRDG

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/edict2_p.h \
    $$PWD/edict2.h

SOURCES += \
    $$PWD/edict2_p.cc \
    $$PWD/edict2.cc

QT += core

# EOF
