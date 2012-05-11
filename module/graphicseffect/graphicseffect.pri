# graphicseffect.pri
# 5/3/2012

include(../../config.pri)
#include($$ROOTDIR/module/pixmapfilter/pixmapfilter.pri)

DEFINES += WITH_MODULE_GRAPHICSEFFECT

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/graphicshaloeffect.h \
    $$PWD/graphicsrippleeffect.h

SOURCES += \
    $$PWD/graphicshaloeffect.cc \
    $$PWD/graphicsrippleeffect.cc

QT      += core gui

# EOF
