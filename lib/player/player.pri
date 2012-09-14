# player.pri
# 6/28/2011

include(../../config.pri)
include($$ROOTDIR/lib/vlccore/vlccore.pri)
include($$ROOTDIR/lib/vlchttp/vlchttp.pri)

DEFINES += WITH_LIB_PLAYER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/player_p.h \
    $$PWD/playerdefs.h \
    $$PWD/player.h

SOURCES += \
    $$PWD/player.cc

OTHER_FILES += \
    $$PWD/vlc-help.txt

QT      += core gui network
LIBS    += -lvlc -lvlccore

win32: DEFINES += USE_PLAYER_PLAYLIST

# EOF
