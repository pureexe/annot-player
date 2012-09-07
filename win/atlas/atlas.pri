# atlas.pri
# 9/6/2012

DEFINES += WITH_WIN_ATLAS

INCLUDEPATH     += $$PWD/..
DEPENDPATH      += $$PWD

HEADERS += \
    $$PWD/atlas.h \
    $$PWD/atle_p.h

SOURCES += \
    $$PWD/atlas.cc

QT      += core

# EOF
