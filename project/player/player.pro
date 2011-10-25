# player.pro
# Since 6/30/2011

include(../../config.pri)
include(tr/tr.pri)

## Libraries

include($$ROOTDIR/core/core.pri)
include($$ROOTDIR/module/player/player.pri)
#include($$ROOTDIR/module/doll/doll.pri)
include($$ROOTDIR/module/serveragent/serveragent.pri)
#include($$ROOTDIR/module/clientagent/clientagent.pri)
include($$ROOTDIR/module/gsoap/gsoap.pri)       # would static linking cause license conflicts?
include($$ROOTDIR/module/webbrowser/webbrowser.pri)

#DEFINES += USE_MODULE_DOLL
DEFINES += USE_MODULE_SERVERAGENT
#DEFINES += USE_MODULE_CLIENTAGENT

win32 {
     DEPENDPATH += $$ROOTDIR/core
     DEPENDPATH += $$ROOTDIR/module/mediaplayer
     DEPENDPATH += $$ROOTDIR/module/doll
}

win32 {
    include($$ROOTDIR/win/dwm/dwm.pri)
    include($$ROOTDIR/win/hook/hook.pri)
    include($$ROOTDIR/win/qth/qth.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
    DEFINES += USE_WIN_DWM
    DEFINES += USE_WIN_HOOK
    DEFINES += USE_WIN_QTH
    DEFINES += USE_WIN_QTWIN

    DEPENDPATH += $$ROOTDIR/win/dwm
    DEPENDPATH += $$ROOTDIR/win/hook
    DEPENDPATH += $$ROOTDIR/win/qtwin
    DEPENDPATH += $$ROOTDIR/win/qth
}
mac {
    include($$ROOTDIR/mac/vlckit_qt/vlckit_qt.pri)
    DEFINES += USE_MAC_VLCKIT
}

QT      += core gui sql webkit

## Sources

TEMPLATE    = app
TARGET      = player

win32: CONFIG += windows

SUBPATH = \
    $$PWD \
    $$PWD/annot \
    $$PWD/data \
    $$PWD/db \
    $$PWD/define \
    $$PWD/dialog \
    $$PWD/osd \
    $$PWD/player \
    $$PWD/signal \
    $$PWD/token \
    $$PWD/tr \
    $$PWD/user \
    $$PWD/util \
    $$PWD/web
INCLUDEPATH     += $$SUBPATH
DEPENDPATH      += $$SUBPATH

HEADERS += \
    config.h \
    eventlogger.h \
    mainwindow.h \
    settings.h \
    uistyle.h \
    annot/annotationbrowser.h \
    annot/annotationeditor.h \
    annot/annotationgraphicsitem.h \
    annot/annotationgraphicsview.h \
    annot/annotationgraphicsstyle.h \
    data/datamanager.h \
    db/db_config.h \
    db/db.h \
    define/config.h \
    define/global.h \
    define/rc.h \
    define/stylesheet.h \
    dialog/livedialog.h \
    dialog/logindialog.h \
    dialog/seekdialog.h \
    dialog/syncdialog.h \
    osd/osdconsole.h \
    osd/osdwindow.h \
    osd/videoview.h \
    player/commandlineedit.h \
    player/mainplayer.h \
    player/miniplayer.h \
    player/osdplayer.h \
    player/playerpanel.h \
    player/playerui.h \
    player/prefixlineedit.h \
    player/userlabel.h \
    signal/signalhub.h \
    token/addaliasdialog.h \
    token/tokenview.h \
    user/userpanel.h \
    util/grabber.h \
    util/lineedit.h \
    util/logger.h \
    util/textedit.h \
    web/commentview.h \
    web/trendview.h

SOURCES += \
    eventlogger.cc \
    main.cc \
    mainwindow.cc \
    settings.cc \
    uistyle.cc \
    annot/annotationbrowser.cc \
    annot/annotationeditor.cc \
    annot/annotationgraphicsitem.cc \
    annot/annotationgraphicsview.cc \
    annot/annotationgraphicsstyle.cc \
    data/datamanager.cc \
    db/db.cc \
    dialog/livedialog.cc \
    dialog/logindialog.cc \
    dialog/seekdialog.cc \
    dialog/syncdialog.cc \
    osd/osdconsole.cc \
    osd/osdwindow.cc \
    osd/videoview.cc \
    player/commandlineedit.cc \
    player/mainplayer.cc \
    player/miniplayer.cc \
    player/osdplayer.cc \
    player/playerui.cc \
    player/playerpanel.cc \
    player/prefixlineedit.cc \
    player/userlabel.cc \
    signal/signalhub.cc \
    token/addaliasdialog.cc \
    token/tokenview.cc \
    user/userpanel.cc \
    util/grabber.cc \
    util/lineedit.cc \
    util/logger.cc \
    util/textedit.cc \
    web/commentview.cc \
    web/trendview.cc

win32 {
  DEFINES += USE_MODE_SIGNAL

  HEADERS += \
    signal/messagehandler.h \
    signal/messageview.h \
    signal/processinfo.h \
    signal/processview.h \
    signal/signalview.h

  SOURCES += \
    signal/messagehandler.cc \
    signal/messageview.cc \
    signal/processview.cc \
    signal/signalview.cc
}

RESOURCES += player.qrc

OTHER_FILES += \
    Info.plist \
    player.rc \
    player.ico \
    player.icns

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += player.rc
}

mac {
    ICON = player.icns
    QMAKE_INFO_PLIST = Info.plist
}

# EOF
