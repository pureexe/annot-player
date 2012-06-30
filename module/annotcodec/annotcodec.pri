# annotcodec.pri
# 2/3/2012

DEFINES += WITH_MODULE_ANNOTCODEC

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/acfuncodec.h \
    $$PWD/annotationcodec.h \
    $$PWD/annotationcodecmanager.h \
    $$PWD/bilibilicodec.h \
    $$PWD/nicovideocodec.h

SOURCES += \
    $$PWD/acfuncodec.cc \
    $$PWD/annotationcodec.cc \
    $$PWD/annotationcodecmanager.cc \
    $$PWD/bilibilicodec.cc \
    $$PWD/nicovideocodec.cc

QT      += core network xml script

# EOF
