# player.pri
# Since 6/28/2011

include(../../config.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/player_config.h \
    $$PWD/player.h

SOURCES += \
    $$PWD/player.cc

QT      += core gui
LIBS    += -lvlc

# EOF
