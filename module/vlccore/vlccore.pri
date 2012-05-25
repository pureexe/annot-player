# vlccore.pri
# 6/28/2011

include(../../config.pri)

DEFINES += WITH_MODULE_VLCCORE

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/sound.h \
    $$PWD/video.h

SOURCES += \
    $$PWD/video.cc

QT      += core gui
LIBS    += -lvlc -lvlccore

INCLUDEPATH += \
    $$VLC_SRC \
    $$VLC_SRC/vlc/include

# EOF
