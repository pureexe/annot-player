# translator.pro
# 8/13/2012

VERSION = 0.1.0.5

DEFINES += SRC_TRANSLATOR

include(../../config.pri)
include($$ROOTDIR/src/common/common.pri)

## Libraries

include($$ROOTDIR/lib/animation/animation.pri)
include($$ROOTDIR/lib/crypt/crypt.pri)
include($$ROOTDIR/lib/edrdg/edrdg.pri)
include($$ROOTDIR/lib/download/download.pri)
include($$ROOTDIR/lib/htmlutil/htmlutil.pri)
include($$ROOTDIR/lib/imagefilter/imagefilter.pri)
include($$ROOTDIR/lib/mecab/mecab.pri)
include($$ROOTDIR/lib/mstypes/mstypes.pri)
include($$ROOTDIR/lib/qt/qt.pri)
include($$ROOTDIR/lib/qtx/qtx.pri)
#include($$ROOTDIR/lib/searchengine/searchengine.pri)
include($$ROOTDIR/lib/textcodec/textcodec.pri)
include($$ROOTDIR/lib/translator/translator.pri)

!mac: include($$ROOTDIR/lib/metacall/metacall.pri)

win32 {
    include($$ROOTDIR/win/atlas/atlas.pri)
    include($$ROOTDIR/win/dwm/dwm.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
    include($$ROOTDIR/win/reg/reg.pri)
}
unix: {
    include($$ROOTDIR/unix/qtunix/qtunix.pri)
}
mac {
    include($$ROOTDIR/mac/qtmac/qtmac.pri)
    include($$ROOTDIR/mac/qtcocoa/qtcocoa.pri)
}

win32 {
  #DEFINES += AC_ENABLE_GAME
  #DEFINES += AC_ENABLE_LAUNCHER
  DEFINES += AC_ENABLE_UPDATE
}

QT      += core gui network webkit xml

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
    INSTALLS += target desktop desktop-kde icon

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    desktop-kde.path = $$DATADIR/kde4/apps/solid/actions
    desktop-kde.files += $${TARGET}.desktop

    #service.path = $$DATADIR/dbus-1/services
    #service.files += $${TARGET}.service

    icon.path = $$DATADIR/icons/hicolor/256x256/apps
    icon.files += $${TARGET}.png
}

# EOF
