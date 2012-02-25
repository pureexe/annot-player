# datastream.pri
# 2/9/2012

include(../../config.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/bufferedfifostream.h \
    $$PWD/bufferedremotestream.h \
    $$PWD/bufferedstreampipe.h \
    $$PWD/fifostream.h \
    $$PWD/fileinputstream.h \
    $$PWD/fileoutputstream.h \
    $$PWD/inputstream.h \
    $$PWD/nulloutputstream.h \
    $$PWD/remotestream.h \
    $$PWD/streampipe.h \
    $$PWD/outputstream.h

SOURCES += \
    $$PWD/bufferedfifostream.cc \
    $$PWD/bufferedremotestream.cc \
    $$PWD/bufferedstreampipe.cc \
    $$PWD/inputstream.cc

QT      += core network

# EOF
