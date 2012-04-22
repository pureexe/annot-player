# browser.pro
# 9/3/2011

VERSION = 0.1.1.0

CONFIG += noqt nocrt
include(../../../config.pri)

INCLUDEPATH += ../include
DEPENDPATH  += ../include

## Libraries

#QT              += core gui

## Sources

TEMPLATE    = app
win32: CONFIG += windows
TARGET      = "Annot Browser"

HEADERS += config.h
SOURCES += main.cc

OTHER_FILES += \
    browser.rc \
    browser.ico

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += browser.rc
}

#mac {
#    ICON = app.icns
#    QMAKE_INFO_PLIST = Info.plist
#}


# EOF

