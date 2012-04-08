# player.pro
# 9/3/2011

VERSION = 0.1.4.3

CONFIG += noqt nocrt
include(../../../config.pri)

INCLUDEPATH += ../include
DEPENDPATH  += ../include

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

