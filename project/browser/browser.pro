# browser.pro
# 3/29/2012

VERSION = 0.1.0.0

include(../../config.pri)
include($$ROOTDIR/project/common/common.pri)

## Libraries

include($$ROOTDIR/module/nicoutil/nicoutil.pri)
include($$ROOTDIR/module/qtext/qtext.pri)
include($$ROOTDIR/module/crypt/crypt.pri)
include($$ROOTDIR/module/download/download.pri)

# shared link gave me so many trouble on mac and linux
unix:       include($$ROOTDIR/module/webbrowser/webbrowser_static.pri)
win32:      include($$ROOTDIR/module/webbrowser/webbrowser.pri)

win32 {
    DEFINES += USE_WIN_DWM
    include($$ROOTDIR/win/dwm/dwm.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
}

QT      += core gui network webkit
#CONFIG(static): QTPLUGIN += qsqlite

## Sources

TEMPLATE      = app
win32:  TARGET = annot-browser
unix:   TARGET = annot-browser
mac:   TARGET = "Annot Browser"

win32: CONFIG += windows

INCLUDEPATH     += $$SUBPATH
DEPENDPATH      += $$SUBPATH

HEADERS += \
    application.h \
    global.h \
    mainwindow.h \
    mainwindowprivate.h \
    rc.h \
    settings.h

SOURCES += \
    application.cc \
    main.cc \
    mainwindow.cc \
    settings.cc


RESOURCES += browser.qrc

OTHER_FILES += \
    Info.plist \
    browser.rc \
    browser.ico \
    browser.icns

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += browser.rc
}

mac {
    ICON = browser.icns
    QMAKE_INFO_PLIST = Info.plist
}

## Translations

TRANSLATIONS += \
    $$PWD/tr/tr_en.ts \
    $$PWD/tr/tr_ja.ts \
    $$PWD/tr/tr_zh.ts

CODECFORTR = UTF-8

OTHER_FILES += $$TRANSLATIONS \
    $$PWD/tr/lrelease_tr.cmd \
    $$PWD/tr/lrelease_tr.sh \
    $$PWD/tr/lupdate_tr.cmd \
    $$PWD/tr/lupdate_tr.sh

# EOF

# Deployment
#unix:!mac {
#    INSTALLS += target desktop desktop-kde icon
#
#    target.path = $$BINDIR
#
#    LUA_SCRIPTS.path = $$DATADIR/annot/player/lua
#
#    desktop.path = $$DATADIR/applications
#    desktop.files += $${TARGET}.desktop
#
#    desktop-kde.path = $$DATADIR/kde4/apps/solid/actions
#    desktop-kde.files += $${TARGET}.desktop
#
#    #service.path = $$DATADIR/dbus-1/services
#    #service.files += $${TARGET}.service
#
#    icon.path = $$DATADIR/icons/hicolor/256x256/apps
#    icon.files += $${TARGET}.png
#}
#
