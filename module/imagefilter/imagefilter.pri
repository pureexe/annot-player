# imagefilter.pri
# 5/16/2012

include(../../config.pri)

DEFINES += WITH_MODULE_IMAGEFILTER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/brightnessimagefilter.h \
    $$PWD/contrastimagefilter.h \
    $$PWD/darknessimagefilter.h \
    $$PWD/fogimagefilter.h \
    $$PWD/gammaimagefilter.h \
    $$PWD/glassimagefilter.h \
    $$PWD/imagefilter.h \
    $$PWD/pixelimagefilter.h \
    $$PWD/rippleimagefilter.h \
    $$PWD/simpleimagefilter.h \
    $$PWD/usmimagefilter.h

SOURCES += \
    $$PWD/rippleimagefilter.cc

QT      += core gui

# EOF
