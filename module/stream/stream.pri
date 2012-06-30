# stream.pri
# 2/9/2012

DEFINES += WITH_MODULE_STREAM

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/bufferedfifostream.h \
    $$PWD/bufferedremotestream.h \
    $$PWD/bufferedstreampipe.h \
    $$PWD/datainputstream.h \
    $$PWD/dataoutputstream.h \
    $$PWD/fifostream.h \
    $$PWD/fileinputstream.h \
    $$PWD/fileoutputstream.h \
    $$PWD/filestream.h \
    $$PWD/inputoutputstream.h \
    $$PWD/inputstream.h \
    $$PWD/nulloutputstream.h \
    $$PWD/remotestream.h \
    $$PWD/securebufferedfifostream.h \
    $$PWD/streampipe.h \
    $$PWD/outputstream.h

SOURCES += \
    $$PWD/bufferedfifostream.cc \
    $$PWD/bufferedremotestream.cc \
    $$PWD/bufferedstreampipe.cc \
    $$PWD/datainputstream.cc \
    $$PWD/remotestream.cc \
    $$PWD/securebufferedfifostream.cc \
    $$PWD/inputstream.cc

QT      += core network

# EOF
