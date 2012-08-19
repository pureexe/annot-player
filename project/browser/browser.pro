# browser.pro
# 3/29/2012

VERSION = 0.1.3.4

DEFINES += PROJECT_BROWSER

include(../../config.pri)
include($$ROOTDIR/project/common/common.pri)
include($$ROOTDIR/module/debug/debug.pri)

# Enforce 32bit on Mac until apple releases 64bit version of QuickTime Plugin
mac: CONFIG -= x86_64
mac: CONFIG += x86

## Libraries

include($$ROOTDIR/module/animation/animation.pri)
include($$ROOTDIR/module/annotcache/annotcache.pri)
include($$ROOTDIR/module/annotdown/annotdown.pri)
include($$ROOTDIR/module/compress/compress.pri)
include($$ROOTDIR/module/crypt/crypt.pri)
include($$ROOTDIR/module/download/download.pri)
#include($$ROOTDIR/module/imagefilter/imagefilter.pri)
#include($$ROOTDIR/module/magnifier/magnifier.pri)
include($$ROOTDIR/module/mousegesture/mousegesture.pri)
include($$ROOTDIR/module/mrlresolver/mrlresolver.pri)
include($$ROOTDIR/module/nicoutil/nicoutil.pri)
include($$ROOTDIR/module/qt/qt.pri)
include($$ROOTDIR/module/qtext/qtext.pri)
include($$ROOTDIR/module/webbrowser/webbrowser.pri)

!mac: include($$ROOTDIR/module/metacall/metacall.pri)

win32 {
    include($$ROOTDIR/win/dwm/dwm.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
}
mac {
    include($$ROOTDIR/mac/qtmac/qtmac.pri)
    include($$ROOTDIR/mac/qtcocoa/qtcocoa.pri)
    include($$ROOTDIR/unix/qtunix/qtunix.pri)
}

QT      += core gui network webkit xml
#CONFIG(static): QTPLUGIN += qsqlite

DEFINES += WITH_QT_CORE \
           WITH_QT_GUI \
           WITH_QT_NETWORK \
           WITH_QT_WEBKIT \
           WITH_QT_XML

win32 {
  DEFINES += AC_ENABLE_GAME
  DEFINES += AC_ENABLE_UPDATE
}

## Sources

TEMPLATE      = app
win32:  TARGET = annot-browser
unix:   TARGET = annot-browser
mac:   TARGET = "Annot Browser"

win32: CONFIG += windows

HEADERS += \
    application.h \
    global.h \
    mainwindow.h \
    mainwindow_p.h \
    preferences.h \
    preferences_p.h \
    rc.h \
    settings.h

SOURCES += \
    application.cc \
    main.cc \
    mainwindow.cc \
    preferences.cc \
    preferences_p.cc \
    settings.cc


RESOURCES += browser.qrc

OTHER_FILES += \
    annot-browser.desktop \
    annot-browser.png \
    Info.plist \
    browser.rc \
    browser.ico \
    browser.icns \
    debian.control \
    debian.rules \
    deploy-debian.sh \
    deploy-fedora.sh \
    rpm.spec

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
    INSTALLS += target desktop desktop-kde icon doc jsf image

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    desktop-kde.path = $$DATADIR/kde4/apps/solid/actions
    desktop-kde.files += $${TARGET}.desktop

    #service.path = $$DATADIR/dbus-1/services
    #service.files += $${TARGET}.service

    icon.path = $$DATADIR/icons/hicolor/256x256/apps
    icon.files += $${TARGET}.png

    DOCDIR = $$DATADIR/annot/browser/doc
    doc.path = $$DOCDIR
    doc.files = $$DOC_FILES

    JSFDIR = $$DATADIR/annot/browser/jsf
    jsf.path = $$JSFDIR
    jsf.files = $$JSF_FILES

    IMAGEDIR = $$DATADIR/annot/browser/images
    image.path = $$IMAGEDIR
    image.files = $$IMAGE_FILES
}

# EOF
