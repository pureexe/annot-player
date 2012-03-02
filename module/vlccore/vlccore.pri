# vlccore.pri
# 6/28/2011

include(../../config.pri)

HEADERS += \
    $$PWD/http.h \
    $$PWD/sound.h \
    $$PWD/video.h

SOURCES += \
    $$PWD/http.cc \
    $$PWD/video.cc

DEFINES += \
    HAVE_ZLIB_H \
    HAVE_STRUCT_ADDRINFO

QT      += core gui network
LIBS    += -lvlc -lvlccore -lz

INCLUDEPATH += \
    $$VLC_SRC \
    $$VLC_SRC/vlc/include

# EOF
