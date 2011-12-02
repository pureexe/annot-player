# player.pri
# 6/28/2011

include(../../config.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/player_config.h \
    $$PWD/playerprivate.h \
    $$PWD/player.h \
    $$PWD/vlccore.h \
    $$PWD/vlcext.h

SOURCES += \
    $$PWD/player.cc \
    $$PWD/vlccore.cc

OTHER_FILES += \
    $$PWD/vlc-help.txt

QT      += core gui
LIBS    += -lvlc

win32: { # same as mac
    DEFINES += WITH_VLCCORE
    LIBS    += -lvlccore
}

# EOF
