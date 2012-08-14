# translator.pro
# 8/11/2012

VERSION = 0.1.0.0

CONFIG += noqt nocrt
include(../../../config.pri)
include($$ROOTDIR/project/launcher/launcher.pri)

## Libraries

#QT              += core gui

## Sources

TEMPLATE    = app
win32: CONFIG += windows
TARGET      = "[ Translate ]"

HEADERS += config.h
SOURCES += main.cc

OTHER_FILES += \
    translator.rc \
    translator.ico

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += translator.rc
}

#mac {
#    ICON = app.icns
#    QMAKE_INFO_PLIST = Info.plist
#}


# EOF

