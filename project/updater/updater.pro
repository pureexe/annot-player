# updater.pro
# 8/11/2012

VERSION = 0.1.0.0

CONFIG += noqt nocrt
include(../../config.pri)

## Libraries

#QT              += core gui

## Sources

TEMPLATE    = app
win32: CONFIG += windows
TARGET      = "annot-update"

HEADERS += \
    global.h \
    util.h
SOURCES += \
    main.cc

OTHER_FILES += \
    updater.rc \
    updater.ico

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += updater.rc
}

#mac {
#    ICON = app.icns
#    QMAKE_INFO_PLIST = Info.plist
#}


# EOF
