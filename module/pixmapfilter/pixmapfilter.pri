# pixmapfilter.pri
# 5/3/2012

include(../../config.pri)

DEFINES += WITH_MODULE_PIXMAPFILTER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/pixmapfilter.h \
    $$PWD/pixmaphalofilter.h \
    $$PWD/pixmapripplefilter.h

SOURCES += \
    $$PWD/pixmaphalofilter.cc \
    $$PWD/pixmapripplefilter.cc

QT      += core gui

INCLUDEPATH     += $$QT_SRC

# EOF
