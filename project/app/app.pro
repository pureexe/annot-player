# app.pro
# Since 9/3/2011

include(../../config.pri)

## Libraries

#QT              += core gui

## Sources

TEMPLATE    = app
TARGET      = "Annot Player"

CONFIG -= qt
QT     -= core gui
LIBS   -= -lQtCore -lQtGui

win32 {
  CONFIG += windows
}

HEADERS += \
    config.h

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

mac {
    ICON = app.icns
    QMAKE_INFO_PLIST = Info.plist
}


# EOF

