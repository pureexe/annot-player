# player.pro
# 6/30/2011

DEFINES += PROJECT_PLAYER

VERSION_MAJOR = 0.1.9.
VERSION_MINOR = 1

VERSION = $$VERSION_MAJOR$$VERSION_MINOR

DEFINES += VERSION_MAJOR=\\\"$$VERSION_MAJOR\\\" \
           VERSION_MINOR=\\\"$$VERSION_MINOR\\\"

include(../../config.pri)
include(tr/tr.pri)
include($$ROOTDIR/project/common/common.pri)
include($$ROOTDIR/module/debug/debug.pri)

## Libraries

include($$ROOTDIR/module/animation/animation.pri)
include($$ROOTDIR/module/annotcloud/annotcloud.pri)
include($$ROOTDIR/module/annotcache/annotcache.pri)
include($$ROOTDIR/module/annotcodec/annotcodec.pri)
include($$ROOTDIR/module/annotdb/annotdb.pri)
include($$ROOTDIR/module/annotdown/annotdown.pri)
include($$ROOTDIR/module/annotservice/annotservice.pri)
include($$ROOTDIR/module/blockiodevice/blockiodevice.pri)
include($$ROOTDIR/module/compress/compress.pri)
include($$ROOTDIR/module/crypt/crypt.pri)
include($$ROOTDIR/module/download/download.pri)
include($$ROOTDIR/module/eventlistener/eventlistener.pri)
include($$ROOTDIR/module/graphicseffect/graphicseffect.pri)
include($$ROOTDIR/module/gsoap/gsoap.pri)       # would static linking cause license conflicts?
include($$ROOTDIR/module/imagefilter/imagefilter.pri)
include($$ROOTDIR/module/ioutil/ioutil.pri)
include($$ROOTDIR/module/magnifier/magnifier.pri)
include($$ROOTDIR/module/mediacodec/mediacodec.pri)
include($$ROOTDIR/module/mrlresolver/mrlresolver.pri)
include($$ROOTDIR/module/nicoutil/nicoutil.pri)
include($$ROOTDIR/module/player/player.pri)
include($$ROOTDIR/module/qt/qt.pri)
include($$ROOTDIR/module/qtext/qtext.pri)
include($$ROOTDIR/module/searchengine/searchengine.pri)
include($$ROOTDIR/module/stream/stream.pri)
include($$ROOTDIR/module/textcodec/textcodec.pri)
include($$ROOTDIR/module/translator/translator.pri)

!mac: include($$ROOTDIR/module/metacall/metacall.pri)
win32 {
    include($$ROOTDIR/win/dwm/dwm.pri)
    #include($$ROOTDIR/win/hook/hook.pri)
    include($$ROOTDIR/win/mousehook/mousehook.pri)
    include($$ROOTDIR/win/picker/picker.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
    include($$ROOTDIR/win/qtwinnt/qtwinnt.pri)
    include($$ROOTDIR/win/texthook/texthook.pri)
}
unix: {
    include($$ROOTDIR/unix/qtunix/qtunix.pri)
}
unix:!mac {
    include($$ROOTDIR/unix/qtx/qtx.pri)
}
mac {
    include($$ROOTDIR/mac/qtmac/qtmac.pri)
    include($$ROOTDIR/mac/qtcocoa/qtcocoa.pri)
    include($$ROOTDIR/mac/vlccocoa/vlccocoa.pri)
}

QT      += core gui network sql svg webkit xml
#CONFIG(static): QTPLUGIN += qsqlite

DEFINES += WITH_QT_CORE \
           WITH_QT_GUI \
           WITH_QT_NETWORK \
           WITH_QT_SQL \
           WITH_QT_SVG \
           WITH_QT_WEBKIT \
           WITH_QT_XML

# Increase heap space
#win32 {
#  QMAKE_CXXFLAGS += -Zm2000    # 2000 is the maximum according to MSDN
#  #QMAKE_LFLAGS   += -Zm1000
#}

## Sources

TEMPLATE      = app
win32:  TARGET = annot-player
unix:   TARGET = annot-player
mac:   TARGET = "Annot Player"

win32: CONFIG += windows

MYPATH = \
    $$PWD \
    $$PWD/annot \
    $$PWD/command \
    $$PWD/console \
    $$PWD/data \
    $$PWD/db \
    $$PWD/dialog \
    $$PWD/game \
    $$PWD/global \
    $$PWD/library \
    $$PWD/osd \
    $$PWD/player \
    $$PWD/token \
    $$PWD/tr \
    $$PWD/user \
    $$PWD/util
INCLUDEPATH     += $$MYPATH
DEPENDPATH      += $$MYPATH

HEADERS += \
    annotationprefs.h \
    application.h \
    preferences.h \
    mainwindow.h \
    mainwindow_p.h \
    settings.h \
    signalhub.h \
    textcodecmanager.h \
    tray.h \
    annot/annotationanalyticsview.h \
    annot/annotationeditor.h \
    annot/annotationfilter.h \
    annot/annotationgraphicseffect.h \
    annot/annotationgraphicsitem.h \
    annot/annotationgraphicsitempool.h \
    annot/annotationgraphicsitemscheduler.h \
    annot/annotationgraphicsview.h \
    annot/annotationlistmodel.h \
    annot/annotationlistview.h \
    annot/annotationsettings.h \
    annot/blacklistview.h \
    annot/blacklistview_p.h \
    annot/textformathandler.h \
    command/annotationcomboedit.h \
    command/inputcombobox.h \
    command/prefixcombobox.h \
    console/consoledialog.h \
    console/eventlogger.h \
    console/mainconsole.h \
    console/miniconsole.h \
    data/datamanager.h \
    data/dataserver.h \
    dialog/annotationcountdialog.h \
    dialog/annotdownurldialog.h \
    dialog/annoturldialog.h \
    dialog/audiodelaydialog.h \
    dialog/backlogdialog.h \
    dialog/countdowndialog.h \
    dialog/devicedialog.h \
    dialog/helpdialog.h \
    dialog/infoview.h \
    dialog/inputdialog.h \
    dialog/logindialog.h \
    dialog/mediainfoview.h \
    dialog/mediaurldialog.h \
    dialog/pickdialog.h \
    dialog/secondinputdialog.h \
    dialog/seekdialog.h \
    dialog/shutdowndialog.h \
    dialog/sleepdialog.h \
    dialog/timeinputdialog.h \
    dialog/urldialog.h \
    global/global.h \
    global/rc.h \
    global/stylesheet.h \
    library/game.h \
    library/gamelibrary.h \
    library/gameview.h \
    library/mainlibraryview.h \
    library/media.h \
    library/medialibrary.h \
    library/medialibraryview.h \
    library/mediamodel.h \
    library/textmessage.h \
    library/textthread.h \
    osd/osdwindow.h \
    osd/videoview.h \
    player/mainplayer.h \
    player/mediathumbnail.h \
    player/miniplayer.h \
    player/embeddedcanvas.h \
    player/embeddedinfoview.h \
    player/embeddedplayer.h \
    player/playerpanel.h \
    player/playerui.h \
    player/positioncalibration.h \
    player/positionslider.h \
    player/userlabel.h \
    token/addaliasdialog.h \
    token/tokenview.h \
    user/useranalyticsview.h \
    user/userview.h \
    util/clipboardmonitor.h \
    util/closewidgetthread.h \
    util/grabber.h \
    util/icons.h \
    util/checkboxgrid.h \
    util/radiobuttongrid.h \
    util/textedittabview.h \
    util/textview.h

SOURCES += \
    annotationprefs.cc \
    application.cc \
    main.cc \
    mainwindow.cc \
    mainwindow_log.cc \
    preferences.cc \
    settings.cc \
    signalhub.cc \
    textcodecmanager.cc \
    tray.cc \
    annot/annotationanalyticsview.cc \
    annot/annotationeditor.cc \
    annot/annotationfilter.cc \
    annot/annotationgraphicseffect.cc \
    annot/annotationgraphicsitem.cc \
    annot/annotationgraphicsitempool.cc \
    annot/annotationgraphicsitemscheduler.cc \
    annot/annotationgraphicsview.cc \
    annot/annotationlistmodel.cc \
    annot/annotationlistview.cc \
    annot/annotationsettings.cc \
    annot/blacklistview.cc \
    annot/blacklistview_p.cc \
    annot/textformathandler.cc \
    command/annotationcomboedit.cc \
    command/inputcombobox.cc \
    command/prefixcombobox.cc \
    console/consoledialog.cc \
    console/eventlogger.cc \
    console/mainconsole.cc \
    console/miniconsole.cc \
    data/datamanager.cc \
    data/dataserver.cc \
    dialog/annotationcountdialog.cc \
    dialog/annotdownurldialog.cc \
    dialog/annoturldialog.cc \
    dialog/audiodelaydialog.cc \
    dialog/backlogdialog.cc \
    dialog/countdowndialog.cc \
    dialog/devicedialog.cc \
    dialog/helpdialog.cc \
    dialog/infoview.cc \
    dialog/inputdialog.cc \
    dialog/logindialog.cc \
    dialog/mediainfoview.cc \
    dialog/mediaurldialog.cc \
    dialog/pickdialog.cc \
    dialog/secondinputdialog.cc \
    dialog/seekdialog.cc \
    dialog/shutdowndialog.cc \
    dialog/sleepdialog.cc \
    dialog/timeinputdialog.cc \
    dialog/urldialog.cc \
    library/game.cc \
    library/gamelibrary.cc \
    library/gameview.cc \
    library/mainlibraryview.cc \
    library/mainlibraryview_dock.cc \
    library/media.cc \
    library/medialibrary.cc \
    library/medialibraryview.cc \
    library/mediamodel.cc \
    library/textmessage.cc \
    library/textthread.cc \
    osd/osdwindow.cc \
    osd/videoview.cc \
    player/mainplayer.cc \
    player/mainplayer_dock.cc \
    player/mediathumbnail.cc \
    player/miniplayer.cc \
    player/embeddedcanvas.cc \
    player/embeddedinfoview.cc \
    player/embeddedplayer.cc \
    player/playerui.cc \
    player/playerpanel.cc \
    player/positioncalibration.cc \
    player/positionslider.cc \
    player/userlabel.cc \
    token/addaliasdialog.cc \
    token/tokenview.cc \
    user/useranalyticsview.cc \
    user/userview.cc \
    util/clipboardmonitor.cc \
    util/grabber.cc \
    util/icons.cc \
    util/checkboxgrid.cc \
    util/radiobuttongrid.cc \
    util/textedittabview.cc \
    util/textview.cc

win32 {
  HEADERS += associationprefs.h windowsregistry.h
  SOURCES += associationprefs.cc windowsregistry.cc

  DEFINES += AC_ENABLE_GAME
  DEFINES += AC_ENABLE_LAUNCHER
  DEFINES += AC_ENABLE_UPDATE

  HEADERS += \
    game/messagehandler.h \
    game/messageview.h \
    game/processinfo.h \
    game/processfilter.h \
    game/processview.h \
    game/syncview.h

  SOURCES += \
    game/messagehandler.cc \
    game/messageview.cc \
    game/processinfo.cc \
    game/processfilter.cc \
    game/processview.cc \
    game/syncview.cc
}

RESOURCES += player.qrc

OTHER_FILES += \
    annot-player.desktop \
    annot-player.png \
    debian.rules \
    debian.control \
    deploy-debian.sh \
    deploy-fedora.sh \
    Info.plist \
    player.rc \
    player.ico \
    player.icns \
    rpm.spec

AVATAR_FILES += \
    avatars/user_0.jpg avatars/user_1.jpg avatars/user_2.jpg avatars/user_3.jpg avatars/user_4.jpg avatars/user_5.jpg avatars/user_6.jpg avatars/user_7.jpg avatars/user_8.jpg avatars/user_9.jpg \
    avatars/user_10.jpg avatars/user_11.jpg avatars/user_12.jpg avatars/user_13.jpg avatars/user_14.jpg avatars/user_15.jpg avatars/user_16.jpg avatars/user_17.jpg avatars/user_18.jpg avatars/user_19.jpg \
    avatars/user_20.jpg avatars/user_21.jpg avatars/user_22.jpg avatars/user_23.jpg avatars/user_24.jpg avatars/user_25.jpg avatars/user_26.jpg avatars/user_27.jpg avatars/user_28.jpg avatars/user_29.jpg \
    avatars/user_30.jpg avatars/user_31.jpg avatars/user_32.jpg avatars/user_33.jpg avatars/user_34.jpg avatars/user_35.jpg avatars/user_36.jpg avatars/user_37.jpg avatars/user_38.jpg avatars/user_39.jpg \
    avatars/user_40.jpg avatars/user_41.jpg avatars/user_42.jpg avatars/user_43.jpg avatars/user_44.jpg avatars/user_45.jpg avatars/user_46.jpg avatars/user_47.jpg avatars/user_48.jpg avatars/user_49.jpg \
    avatars/user_50.jpg avatars/user_51.jpg avatars/user_52.jpg avatars/user_53.jpg avatars/user_54.jpg avatars/user_55.jpg avatars/user_56.jpg avatars/user_57.jpg avatars/user_58.jpg avatars/user_59.jpg \
    avatars/user_60.jpg avatars/user_61.jpg avatars/user_62.jpg avatars/user_63.jpg avatars/user_64.jpg avatars/user_65.jpg avatars/user_66.jpg avatars/user_67.jpg avatars/user_68.jpg avatars/user_69.jpg \
    avatars/user_70.jpg avatars/user_71.jpg avatars/user_72.jpg avatars/user_73.jpg avatars/user_74.jpg avatars/user_75.jpg avatars/user_76.jpg avatars/user_77.jpg avatars/user_78.jpg avatars/user_79.jpg \
    avatars/user_80.jpg avatars/user_81.jpg avatars/user_82.jpg avatars/user_83.jpg avatars/user_84.jpg avatars/user_85.jpg avatars/user_86.jpg avatars/user_87.jpg avatars/user_88.jpg avatars/user_89.jpg \
    avatars/user_90.jpg avatars/user_91.jpg avatars/user_92.jpg avatars/user_93.jpg avatars/user_94.jpg avatars/user_95.jpg avatars/user_96.jpg avatars/user_97.jpg avatars/user_98.jpg avatars/user_99.jpg \
    avatars/user_100.jpg avatars/user_101.jpg avatars/user_102.jpg avatars/user_103.jpg avatars/user_104.jpg avatars/user_105.jpg avatars/user_106.jpg avatars/user_107.jpg avatars/user_108.jpg avatars/user_109.jpg \
    avatars/user_110.jpg avatars/user_111.jpg avatars/user_112.jpg avatars/user_113.jpg avatars/user_114.jpg avatars/user_115.jpg avatars/user_116.jpg avatars/user_117.jpg avatars/user_118.jpg avatars/user_119.jpg \
    avatars/user_120.jpg avatars/user_121.jpg avatars/user_122.jpg avatars/user_123.jpg avatars/user_124.jpg avatars/user_125.jpg avatars/user_126.jpg avatars/user_127.jpg avatars/user_128.jpg avatars/user_129.jpg \
    avatars/user_130.jpg avatars/user_131.jpg avatars/user_132.jpg avatars/user_133.jpg avatars/user_134.jpg avatars/user_135.jpg
DEFINES += AVATAR_USER_COUNT=136

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += player.rc
}

mac {
    ICON = player.icns
    QMAKE_INFO_PLIST = Info.plist
}

# Deployment

unix:!mac {
    INSTALLS += target desktop desktop-kde icon lua doc jsf avatar image

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    desktop-kde.path = $$DATADIR/kde4/apps/solid/actions
    desktop-kde.files += $${TARGET}.desktop

    #service.path = $$DATADIR/dbus-1/services
    #service.files += $${TARGET}.service

    icon.path = $$DATADIR/icons/hicolor/256x256/apps
    icon.files += $${TARGET}.png

    LUADIR = $$DATADIR/annot/player/lua
    lua.path = $$LUADIR
    lua.files = $$LUA_FILES

    DOCDIR = $$DATADIR/annot/player/doc
    doc.path = $$DOCDIR
    doc.files = $$DOC_FILES

    JSFDIR = $$DATADIR/annot/player/jsf
    jsf.path = $$JSFDIR
    jsf.files = $$JSF_FILES

    AVATARDIR = $$DATADIR/annot/player/avatars
    avatar.path = $$AVATARDIR
    avatar.files = $$AVATAR_FILES

    IMAGEDIR = $$DATADIR/annot/player/images
    image.path = $$IMAGEDIR
    image.files = $$IMAGE_FILES
}

# EOF
