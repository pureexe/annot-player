# imagefilter.pri
# 5/16/2012

DEFINES += WITH_LIB_IMAGEFILTER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/brightnessimagefilter.h \
    $$PWD/contrastimagefilter.h \
    $$PWD/darknessimagefilter.h \
    $$PWD/filters.h \
    $$PWD/fogimagefilter.h \
    $$PWD/gammaimagefilter.h \
    $$PWD/glassimagefilter.h \
    $$PWD/imagefilter.h \
    $$PWD/pixelimagefilter.h \
    $$PWD/qtfilters.h \
    $$PWD/rippleimagefilter.h \
    $$PWD/simpleimagefilter.h \
    $$PWD/usmimagefilter.h

SOURCES += \
    $$PWD/filters.cc \
    $$PWD/rippleimagefilter.cc

QT      += core gui
INCLUDEPATH     += $$QT_SRC

# EOF
