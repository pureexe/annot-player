# annotcodec.pri
# 2/3/2012

include(../../config.pri)
#include($$ROOTDIR/module/compress/compress.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/acfuncodec.h \
    $$PWD/annotationcodec.h \
    $$PWD/annotationcodecmanager.h \
    $$PWD/bilibilicodec.h

SOURCES += \
    $$PWD/acfuncodec.cc \
    $$PWD/annotationcodecmanager.cc \
    $$PWD/bilibilicodec.cc

QT      += core network xml script

# EOF
