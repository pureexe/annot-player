# downloader.pro
# 3/29/2012

VERSION = 0.1.2.4

DEFINES += PROJECT_DOWNLOADER

include(../../config.pri)
include($$ROOTDIR/project/common/common.pri)
include($$ROOTDIR/module/debug/debug.pri)

## Libraries

include($$ROOTDIR/module/animation/animation.pri)
include($$ROOTDIR/module/annotcache/annotcache.pri)
include($$ROOTDIR/module/annotcloud/annotcloud.pri)
include($$ROOTDIR/module/annotdown/annotdown.pri)
include($$ROOTDIR/module/annotservice/annotservice.pri)
include($$ROOTDIR/module/compress/compress.pri)
include($$ROOTDIR/module/crypt/crypt.pri)
include($$ROOTDIR/module/download/download.pri)
include($$ROOTDIR/module/downloadtask/downloadtask.pri)
include($$ROOTDIR/module/gsoap/gsoap.pri)
include($$ROOTDIR/module/imagefilter/imagefilter.pri)
include($$ROOTDIR/module/mediacodec/mediacodec.pri)
include($$ROOTDIR/module/mrlresolver/mrlresolver.pri)
include($$ROOTDIR/module/qt/qt.pri)
include($$ROOTDIR/module/qtext/qtext.pri)
include($$ROOTDIR/module/searchengine/searchengine.pri)
include($$ROOTDIR/module/stream/stream.pri)

!mac: include($$ROOTDIR/module/metacall/metacall.pri)

win32 {
    include($$ROOTDIR/win/dwm/dwm.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
}
unix: {
    include($$ROOTDIR/unix/qtunix/qtunix.pri)
}
mac {
    include($$ROOTDIR/mac/qtmac/qtmac.pri)
    include($$ROOTDIR/mac/qtcocoa/qtcocoa.pri)
}

QT      += core gui network xml
#CONFIG(static): QTPLUGIN += qsqlite

DEFINES += WITH_QT_CORE \
           WITH_QT_GUI \
           WITH_QT_NETWORK \
           WITH_QT_XML

win32 {
  DEFINES += AC_ENABLE_GAME
  DEFINES += AC_ENABLE_UPDATE
}

## Sources

TEMPLATE      = app
win32:  TARGET = annot-down
unix:   TARGET = annot-down
mac:   TARGET = "Annot Downloader"

win32: CONFIG += windows

HEADERS += \
    application.h \
    clipboardmonitor.h \
    global.h \
    mainwindow.h \
    mainwindow_p.h \
    rc.h \
    settings.h \
    signer.h \
    taskdialog.h \
    trayicon.h

SOURCES += \
    application.cc \
    clipboardmonitor.cc \
    main.cc \
    mainwindow.cc \
    settings.cc \
    signer.cc \
    taskdialog.cc \
    trayicon.cc

RESOURCES += downloader.qrc

OTHER_FILES += \
    annot-down.desktop \
    annot-down.png \
    Info.plist \
    debian.control \
    debian.rules \
    deploy-debian.sh \
    deploy-fedora.sh \
    downloader.rc \
    downloader.ico \
    downloader.icns \
    rpm.spec

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
    $$PWD/tr/tr_zh_TW.ts \
    $$PWD/tr/tr_zh_CN.ts

CODECFORTR = UTF-8

OTHER_FILES += $$TRANSLATIONS \
    $$PWD/tr/lrelease_tr.cmd \
    $$PWD/tr/lrelease_tr.sh \
    $$PWD/tr/lupdate_tr.cmd \
    $$PWD/tr/lupdate_tr.sh

# Deployment

unix:!mac {
    INSTALLS += target desktop desktop-kde icon lua doc #avatar

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    desktop-kde.path = $$DATADIR/kde4/apps/solid/actions
    desktop-kde.files += $${TARGET}.desktop

    #service.path = $$DATADIR/dbus-1/services
    #service.files += $${TARGET}.service

    icon.path = $$DATADIR/icons/hicolor/256x256/apps
    icon.files += $${TARGET}.png

    LUADIR = $$DATADIR/annot/down/lua
    lua.path = $$LUADIR
    lua.files = $$LUA_FILES

    DOCDIR = $$DATADIR/annot/down/doc
    doc.path = $$DOCDIR
    doc.files = $$DOC_FILES

    AVATARDIR = $$DATADIR/annot/downloader/avatars
    avatar.path = $$AVATARDIR
    avatar.files = $$AVATAR_FILES
}

# EOF
