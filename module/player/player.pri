# player.pri
# 6/28/2011

include(../../config.pri)
include($$ROOTDIR/module/vlccore/vlccore.pri)

DEFINES += WITH_MODULE_PLAYER

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/player_config.h \
    $$PWD/player_p.h \
    $$PWD/player.h

SOURCES += \
    $$PWD/player.cc

OTHER_FILES += \
    $$PWD/vlc-help.txt

QT      += core gui network
LIBS    += -lvlc -lvlccore

win32: DEFINES += USE_PLAYER_PLAYLIST

# EOF
