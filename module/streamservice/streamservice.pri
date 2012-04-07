# streamservice.pri
# 2/11/2012

include(../../config.pri)
include($$ROOTDIR/module/mediacodec/mediacodec.pri)

DEFINES += WITH_MODULE_STREAMSERVICE

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/adtsaudiostreamsource.h \
    $$PWD/adtsaudiostreamsmss.h \
    $$PWD/framedstreamsource.h \
    $$PWD/h264videostreamsmss.h \
    $$PWD/live555_config.h \
    $$PWD/streamservice.h \
    $$PWD/streamsmss.h \
    $$PWD/streamthread.h

SOURCES += \
    $$PWD/adtsaudiostreamsource.cc \
    $$PWD/adtsaudiostreamsmss.cc \
    $$PWD/framedstreamsource.cc \
    $$PWD/h264videostreamsmss.cc \
    $$PWD/streamservice.cc \
    $$PWD/streamsmss.cc \
    $$PWD/streamthread.cc

QT      += core
LIBS   += \
     -lliveMedia \
     -lgroupsock \
     -lUsageEnvironment \
     -lBasicUsageEnvironment

# EOF
