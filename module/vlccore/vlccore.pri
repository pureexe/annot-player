# vlccore.pri
# 6/28/2011

include(../../config.pri)

DEPENDPATH += $$PWD/types

HEADERS += \
    $$PWD/types/media_player.h \
    $$PWD/types/module.h \
    $$PWD/http.h \
    $$PWD/video.h

SOURCES += \
    $$PWD/http.cc \
    $$PWD/video.cc

DEFINES += \
    HAVE_ZLIB_H \
    HAVE_STRUCT_ADDRINFO

QT      += core network
LIBS    += -lvlc -lvlccore -lz

INCLUDEPATH     += $$VLC_SRC/vlc/include
!mac: DEFINES   += USE_MODULE_VLCCORE_HTTP

# EOF
