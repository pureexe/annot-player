# app.pro
# 9/3/2011

VERSION = 0.1.2.9

CONFIG += noqt nocrt
include(../../config.pri)

## Libraries

#QT              += core gui

## Sources

TEMPLATE    = app
win32: CONFIG += windows
TARGET      = "Annot Player"

HEADERS += \
    config.h \
    mscex.h

SOURCES += \
    main.cc

#RESOURCES += app.qrc

OTHER_FILES += \
    Info.plist \
    app.rc \
    app.ico \
    app.icns

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += app.rc
}

#mac {
#    ICON = app.icns
#    QMAKE_INFO_PLIST = Info.plist
#}


# EOF

