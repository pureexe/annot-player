# translator.pro
# 8/13/2012

VERSION = 0.1.0.2

DEFINES += PROJECT_TRANSLATOR

include(../../config.pri)
include($$ROOTDIR/project/common/common.pri)
include($$ROOTDIR/module/debug/debug.pri)

## Libraries

include($$ROOTDIR/module/animation/animation.pri)
include($$ROOTDIR/module/crypt/crypt.pri)
include($$ROOTDIR/module/download/download.pri)
include($$ROOTDIR/module/imagefilter/imagefilter.pri)
include($$ROOTDIR/module/qt/qt.pri)
include($$ROOTDIR/module/qtext/qtext.pri)
include($$ROOTDIR/module/searchengine/searchengine.pri)
include($$ROOTDIR/module/translator/translator.pri)

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
win32:  TARGET = annot-dict
unix:   TARGET = annot-dict
mac:   TARGET = "Annot Translator"

win32: CONFIG += windows

HEADERS += \
    application.h \
    global.h \
    mainwindow.h \
    rc.h \
    settings.h \
    translatebrowser.h \
    translateedit.h \
    trayicon.h

SOURCES += \
    application.cc \
    main.cc \
    mainwindow.cc \
    settings.cc \
    translatebrowser.cc \
    translateedit.cc \
    trayicon.cc

RESOURCES += translator.qrc

OTHER_FILES += \
    annot-dict.desktop \
    annot-dict.png \
    Info.plist \
    debian.control \
    debian.rules \
    deploy-debian.sh \
    deploy-fedora.sh \
    translator.rc \
    translator.ico \
    translator.icns \
    rpm.spec

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += translator.rc
}

mac {
    ICON = translator.icns
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
    INSTALLS += target desktop desktop-kde icon doc 

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    desktop-kde.path = $$DATADIR/kde4/apps/solid/actions
    desktop-kde.files += $${TARGET}.desktop

    #service.path = $$DATADIR/dbus-1/services
    #service.files += $${TARGET}.service

    icon.path = $$DATADIR/icons/hicolor/256x256/apps
    icon.files += $${TARGET}.png

    DOCDIR = $$DATADIR/annot/dict/doc
    doc.path = $$DOCDIR
    doc.files = $$DOC_FILES
}

# EOF
