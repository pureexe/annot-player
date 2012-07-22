# graphicseffect.pri
# 5/3/2012

include(../../config.pri)
include($$ROOTDIR/module/pixmapfilter/pixmapfilter.pri)

DEFINES += WITH_MODULE_GRAPHICSEFFECT

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/graphicseffect_p.h \
    $$PWD/graphicshaloeffect.h \
    $$PWD/graphicshaloeffect_p.h \
    $$PWD/graphicsrippleeffect.h

SOURCES += \
    $$PWD/graphicshaloeffect.cc \
    $$PWD/graphicsrippleeffect.cc

INCLUDEPATH     += $$QT_SRC
QT      += core gui

# EOF
