# downloader.pro
# 3/29/2012

VERSION = 0.1.0.0

include(../../config.pri)
include($$ROOTDIR/project/common/common.pri)

## Libraries

include($$ROOTDIR/module/annotcloud/annotcloud.pri)
include($$ROOTDIR/module/serveragent/serveragent.pri)
include($$ROOTDIR/module/gsoap/gsoap.pri)
include($$ROOTDIR/module/ioutil/ioutil.pri)
include($$ROOTDIR/module/qtext/qtext.pri)
include($$ROOTDIR/module/crypt/crypt.pri)
include($$ROOTDIR/module/download/download.pri)
include($$ROOTDIR/module/download/mrldownload.pri)
include($$ROOTDIR/module/mrlresolver/mrlresolver.pri)
include($$ROOTDIR/module/stream/stream.pri)
include($$ROOTDIR/module/mediacodec/mediacodec.pri)

DEFINES += USE_MODULE_IOUTIL

win32 {
    DEFINES += USE_WIN_DWM
    include($$ROOTDIR/win/dwm/dwm.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
}
unix: {
    include($$ROOTDIR/unix/qtunix/qtunix.pri)
}

QT      += core gui network
#CONFIG(static): QTPLUGIN += qsqlite

## Sources

TEMPLATE      = app
win32:  TARGET = annot-down
unix:   TARGET = annot-down
mac:   TARGET = "Annot Downloader"

win32: CONFIG += windows

INCLUDEPATH     += $$SUBPATH
DEPENDPATH      += $$SUBPATH

HEADERS += \
    application.h \
    clipboardmonitor.h \
    global.h \
    mainwindow.h \
    mainwindowprivate.h \
    rc.h \
    settings.h \
    signer.h \
    taskdialog.h

SOURCES += \
    application.cc \
    clipboardmonitor.cc \
    main.cc \
    mainwindow.cc \
    settings.cc \
    signer.cc \
    taskdialog.cc


RESOURCES += downloader.qrc

OTHER_FILES += \
    Info.plist \
    downloader.rc \
    downloader.ico \
    downloader.icns

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += downloader.rc
}

mac {
    ICON = downloader.icns
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
