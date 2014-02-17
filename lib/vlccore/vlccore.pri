# vlccore.pri
# 6/28/2011

DEFINES += WITH_LIB_VLCCORE

# Cancel building pthread support on Windows
win32: DEFINES += LIBVLC_USE_PTHREAD_CANCEL

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
