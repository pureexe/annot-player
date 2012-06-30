# player.pro
# 9/3/2011

VERSION = 0.1.5.1

CONFIG += noqt nocrt
include(../../../config.pri)
include($$ROOTDIR/project/launcher/launcher.pri)

## Libraries

#QT              += core gui

## Sources

TEMPLATE    = app
win32: CONFIG += windows
TARGET      = "Annot Player"

HEADERS += config.h
SOURCES += main.cc

OTHER_FILES += \
    player.rc \
    player.ico

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += player.rc
}

#mac {
#    ICON = app.icns
#    QMAKE_INFO_PLIST = Info.plist
#}


# EOF

