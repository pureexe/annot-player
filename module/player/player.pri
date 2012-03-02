# player.pri
# 6/28/2011

include(../../config.pri)
include($$ROOTDIR/module/vlccore/vlccore.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/player_config.h \
    $$PWD/playerprivate.h \
    $$PWD/player.h

SOURCES += \
    $$PWD/player.cc

OTHER_FILES += \
    $$PWD/vlc-help.txt

QT      += core gui network
LIBS    += -lvlc -lvlccore

!mac: DEFINES += USE_PLAYER_PLAYLIST

# EOF
