# graphicseffect.pri
# 5/3/2012

include(../../config.pri)
include($$ROOTDIR/lib/pixmapfilter/pixmapfilter.pri)

DEFINES += WITH_LIB_GRAPHICSEFFECT

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
