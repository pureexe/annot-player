# graphicseffect.pri
# 5/3/2012

include(../../config.pri)
#include($$ROOTDIR/module/pixmapfilter/pixmapfilter.pri)

DEFINES += WITH_MODULE_GRAPHICSEFFECT

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/graphicsrippleeffect.h \
    $$PWD/graphicsshadoweffect.h

SOURCES += \
    $$PWD/graphicsrippleeffect.cc \
    $$PWD/graphicsshadoweffect.cc

QT      += core gui

# EOF
