# pixmapfilter.pri
# 5/3/2012

DEFINES += WITH_MODULE_PIXMAPFILTER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/pixmapfilter.h \
    $$PWD/pixmaphalofilter.h

SOURCES += \
    $$PWD/pixmaphalofilter.cc

QT      += core gui

INCLUDEPATH     += $$QT_SRC

# EOF
