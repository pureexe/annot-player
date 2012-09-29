# config.pri
# 9/3/2011 jichi

## Locations

ROOTDIR = $$PWD
BUILDDIR = $$ROOTDIR/../../annot-build

win32:  DESTDIR         = $$BUILDDIR/win
unix:   DESTDIR         = $$BUILDDIR/unix
mac:    DESTDIR         = $$BUILDDIR/mac
#win32:  DESTDIR_TARGET  = $$BUILDDIR/release.win
#unix:   DESTDIR_TARGET  = $$BUILDDIR/release.unix
#mac:    DESTDIR_TARGET  = $$BUILDDIR/release.mac

LIBS            += -L$$DESTDIR
mac:  LIBS      += -F$$DESTDIR

INCLUDEPATH     += $$ROOTDIR
INCLUDEPATH     += $$ROOTDIR/lib

win32:  INCLUDEPATH     += $$ROOTDIR/win
unix:   INCLUDEPATH     += $$ROOTDIR/unix
mac:    INCLUDEPATH     += $$ROOTDIR/mac

INCLUDEPATH += \
    $$ROOTDIR/lib \
    $$ROOTDIR/lib/portable \
    $$ROOTDIR/lib/win

## Compiling options

mac:    CONFIG -= ppc ppc64

DEFINES += UNICODE

#CONFIG += x86 x86_64 ppc64

# C++11
win32:  QMAKE_CXXFLAGS += -Zc:auto
unix:   QMAKE_CXXFLAGS += -std=c++0x
mac    {
    # Enable TR1 such as tuple
    # Clang is required to take place of llvm gcc, which uses /usr/lib/libstdc++.dylib
    #QMAKE_CXXFLAGS += -stdlib=libc++
    #QMAKE_LFLAGS += -stdlib=libc++
}

# MSVC
win32 {
    # Disable checked iterator and compiler warning.
    # SCL: http://msdn.microsoft.com/en-us/library/aa985896.aspx
    # SCL Warning: http://msdn.microsoft.com/en-us/library/aa985974.aspx
    # Performance comparison: http://askldjd.wordpress.com/2009/09/13/stl-performance-comparison-vc71-vc90-and-stlport/
    DEFINES += _SECURE_SCL=0 _SCL_SECURE_NO_WARNINGS
}

# GCC

#QMAKE_LFLAGS +=

## Config

CONFIG(release) {
  #DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
}

CONFIG(noqt) {
  CONFIG -= qt
  QT     -= core gui
  LIBS   -= -lQtCore -lQtGui
}

CONFIG(nogui) {
  QT     -= gui
  LIBS   -= -lQtGui
  mac: CONFIG -= app_bundle
}

CONFIG(nocrt) {
  win32 {
    QMAKE_CFLAGS                -= -MD -MDd
    QMAKE_CFLAGS_DEBUG          -= -MD -MDd
    QMAKE_CFLAGS_RELEASE        -= -MD -MDd
    QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO -= -MD -MDd
    QMAKE_CXXFLAGS              -= -MD -MDd
    QMAKE_CXXFLAGS_DEBUG        -= -MD -MDd
    QMAKE_CXXFLAGS_RELEASE      -= -MD -MDd
    QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -MD -MDd
  }
}

## Translation

CODECFORTR = UTF-8

## Debug

#include($$ROOTDIR/lib/debug/debug.pri)

## Deploy

#DEFINES += VERSION=\\\"$$VERSION\\\"

## Domains and accounts

DEFINES += CONFIG_NICOVIDEO_USERNAME=\\\"oedivocin@gmail.com\\\" \
           CONFIG_NICOVIDEO_PASSWORD=\\\"nicovideo.jp\\\"
DEFINES += CONFIG_BILIBILI_USERNAME=\\\"oedivocin@gmail.com\\\" \
           CONFIG_BILIBILI_PASSWORD=\\\"nicovideo.jp\\\"

#DEFINES += ANNOT_PROXY_DOMAIN=\\\"sakuradite.com\\\"
DEFINES += CONFIG_PROXY_DOMAIN=\\\"210.175.52.140\\\"
DEFINES += CONFIG_ANNOT_IP=\\\"210.175.54.32\\\" \
           CONFIG_ANNOT_DOC_IP=\\\"210.175.52.140\\\"

## IDs

# Azure Market Key: https://datamarket.azure.com/account/datasets
DEFINES += CONFIG_AZURE_ID=\\\"tuSmXew4CSnnGaX0vZyYdNLCrlInvAUepCX6p5l5THc=\\\"

# Yahoo!JAPAN AppID: https://e.developer.yahoo.co.jp/dashboard/
DEFINES += CONFIG_YAHOO_ID=\\\"mRr.UCWxg65ZTZTR_Mz0OTtj3sJ7xa5K66ZOGp55cgJsIDDeaB6e1LDY1NpEZ_AfZA--\\\"

## Deploy
# See: http://wiki.maemo.org/Packaging_a_Qt_application
# See: http://www.developer.nokia.com/Community/Wiki/Creating_Debian_packages_for_Maemo_5_Qt_applications_and_showing_in_the_application_menu
# See: https://wiki.kubuntu.org/PackagingGuide/QtApplication

#QMLDIR=$$DESTDIR/qml
#LUADIR=$$DESTDIR/lua
#DOCDIR=$$DESTDIR/doc
#TABLEDIR=$$DESTDIR/table
#IMAGEDIR=$$DESTDIR/images
#JSFDIR=$$DESTDIR/jsf
#AVATARDIR=$$DESTDIR/avatars

#unix:!mac {
#    isEmpty(PREFIX): PREFIX = /usr
#    BINDIR = $$PREFIX/bin
#    DATADIR = $$PREFIX/share
#}
#mac {
#    isEmpty(PREFIX): PREFIX = /opt/annot
#    BINDIR = $$PREFIX/bin
#    DATADIR = $$PREFIX/share
#}
#
#!win32 {
#  DEFINES += \
#    BINDIR=\\\"$$BINDIR\\\" \
#    DATADIR=\\\"$$DATADIR\\\"
#}
#DEFINES += \
#    AVATARDIR=\\\"$$AVATARDIR\\\" \
#    DOCDIR=\\\"$$DOCDIR\\\" \
#    TABLEDIR=\\\"$$TABLEDIR\\\" \
#    IMAGEDIR=\\\"$$IMAGDIR\\\" \
#    LUADIR=\\\"$$LUADIR\\\" \
#    JSFDIR=\\\"$$JSFDIR\\\"

## External libraries

#win32 {
#    LIB_HOME            = c:/dev
#    #LIB_HOME            = B:/Developer
#    QT_HOME             = c:/qt/qt4
#    QT_SRC              = c:/qt
#    QT5_HOME            = c:/qt/qt5
#    QT5_SRC             = c:/qt
#    #VLC_HOME            = "c:/Program Files/VideoLAN/VLC/sdk"
#    VLC_HOME            = $$LIB_HOME/vlc
#    VLC_SRC             = $$VLC_HOME/src
#    #WSF_HOME            = $$LIB_HOME/wso2
#    #CDIO_HOME          = $$LIB_HOME/cdio
#    #FFMPEG_HOME         = $$LIB_HOME/ffmpeg
#    GPAC_HOME           = $$LIB_HOME/gpac
#    MP4V2_HOME          = $$LIB_HOME/mp4v2
#    LIVE_HOME           = $$LIB_HOME/live
#    POPPLER_HOME        = $$LIB_HOME/poppler
#    BOOST_HOME          = $$LIB_HOME/boost
#    GSOAP_HOME          = $$LIB_HOME/gsoap
#    GSOAP_SRC           = $$LIB_HOME/gsoap/src
#    ZLIB_HOME           = $$LIB_HOME/zlib
#    LUA_HOME            = $$LIB_HOME/lua
#    FREETYPE_HOME       = $$LIB_HOME/freetype
#    FONTCONFIG_HOME     = $$LIB_HOME/fontconfig
#    MECAB_HOME          = $$LIB_HOME/mecab
#    #LUA_VERSION = 52
#    #LUA_VERSION = 5.1
#    LUA_VERSION =
#
#    ITH_HOME            = $$LIB_HOME/ith
#
#    WDK_HOME            = c:/winddk/current
#
#    INCLUDEPATH        += $$LIB_HOME/inttypes/include
#
#    INCLUDEPATH        += $$ITH_HOME/include
#    LIBS               += -L$$ITH_HOME/lib
#
#    # wdk/inc/api/sal.h MUST be removed.
#    # See:  http://stackoverflow.com/questions/1356653/multiple-compiling-errors-with-basic-c-application-on-vs2010-beta-1
#    # Select WinXP x86
#    INCLUDEPATH        += $$WDK_HOME/inc
#    LIBS               += -L$$WDK_HOME/lib/wxp/i386
#}
#
#unix {
#    X11_HOME            = /usr/X11
#    QT_HOME             = /usr/share/qt4
#    QT_SRC              =
#    VLC_HOME            = /usr
#    VLC_SRC             = ${HOME}/opt/src
#    #WSF_HOME            = ${HOME}/opt/wso2/wsf
#    #CDIO_HOME          = /usr
#    #FFMPEG_HOME         = /usr
#    MECAB_HOME          = /usr
#    GPAC_HOME           = /usr
#    MP4V2_HOME          = /usr
#    LIVE_HOME           = /usr
#    POPPLER_HOME        = ${HOME}/opt/poppler
#    BOOST_HOME          = /usr
#    GSOAP_HOME          = ${HOME}/opt/gsoap
#    GSOAP_SRC           = ${HOME}/opt/src/gsoap
#    LUA_HOME            = /usr
#    ZLIB_HOME           = /usr
#    FREETYPE_HOME       = $$X11_HOME
#    FONTCONFIG_HOME     = $$X11_HOME
#    LUA_VERSION = 5.1
#}
#
#mac {
#    SDK_HOME            = /Developer/SDKs/MacOSX10.7.sdk
#    X11_HOME            = $$SDK_HOME/usr/X11
#    MACPORTS_HOME       = /opt/local
#    QT_HOME             = ${HOME}/opt/qt
#    QT_SRC              = ${HOME}/opt/src
#    QT5_HOME            = ${HOME}/opt/qt5
#    QT5_SRC             = ${HOME}/opt/src
#    #VLC_HOME            = ${HOME}/opt/vlc
#    VLC_HOME            = /Applications/VLC.app/Contents/MacOS
#    VLC_SRC             = ${HOME}/opt/src
#    #WSF_HOME           = ${HOME}/opt/wso2/wsf
#    #CDIO_HOME          = ${HOME}/opt/libcdio
#    #FFMPEG_HOME         = $$MACPORTS_HOME
#    GPAC_HOME           = ${HOME}/opt/gpac
#    MP4V2_HOME          = $$MACPORTS_HOME
#    MECAB_HOME          = $$MACPORTS_HOME
#    LIVE_HOME           = ${HOME}/opt/live
#    POPPLER_HOME        = ${HOME}/opt/poppler
#    BOOST_HOME          = $$MACPORTS_HOME
#    GSOAP_HOME          = ${HOME}/opt/gsoap
#    GSOAP_SRC           = ${HOME}/opt/src/gsoap
#    ZLIB_HOME           = $$SDK_HOME/usr
#    FREETYPE_HOME       = $$X11_HOME
#    FONTCONFIG_HOME     = $$X11_HOME
#    #LUA_HOME            = ${HOME}/opt/lua
#    LUA_HOME            = $$MACPORTS_HOME
#    #LUA_VERSION = 52
#    LUA_VERSION =
#}
#
#INCLUDEPATH     += $$QT_SRC/qt/src
##INCLUDEPATH     += $$QT5_SRC/qt/src
#
#INCLUDEPATH     += $$VLC_HOME/include
#INCLUDEPATH     += $$VLC_HOME/include/vlc/plugins
##INCLUDEPATH     += $$VLC_SRC/include
#LIBS            += -L$$VLC_HOME/lib
##INCLUDEPATH     += $$WSF_HOME/include
##LIBS            += -L$$WSF_HOME/lib
##INCLUDEPATH     += $$CDIO_HOME/include
##LIBS            += -L$$CDIO_HOME/lib
##INCLUDEPATH     += $$POPPLER_HOME/include/poppler/qt4
##LIBS            += -L$$POPPLER_HOME/lib
#INCLUDEPATH     += $$BOOST_HOME/include
#LIBS            += -L$$BOOST_HOME/lib
#INCLUDEPATH     += $$GSOAP_HOME/include
##LIBS            += -L$$GSOAP_HOME/lib
#INCLUDEPATH     += $$ZLIB_HOME/include
#LIBS            += -L$$ZLIB_HOME/lib
#INCLUDEPATH     += $$MECAB_HOME/include
#LIBS            += -L$$MECAB_HOME/lib
#INCLUDEPATH     += $$FREETYPE_HOME/include \
#                   $$FREETYPE_HOME/include/freetype2
#LIBS            += -L$$FREETYPE_HOME/lib
#INCLUDEPATH     += $$FONTCONFIG_HOME/include
#LIBS            += -L$$FONTCONFIG_HOME/lib
#INCLUDEPATH     += $$LUA_HOME/include \
#                   $$LUA_HOME/include/lua$$LUA_VERSION
#LIBS            += -L$$LUA_HOME/lib
##INCLUDEPATH     += $$FFMPEG_HOME/include
##LIBS            += -L$$FFMPEG_HOME/lib
##INCLUDEPATH     += $$GPAC_HOME/include
##LIBS            += -L$$GPAC_HOME/lib
##INCLUDEPATH     += $$MP4V2_HOME/include
##LIBS            += -L$$MP4V2_HOME/lib
##INCLUDEPATH     += \
##    $$LIVE_HOME/BasicUsageEnvironment/include \
##    $$LIVE_HOME/UsageEnvironment/include \
##    $$LIVE_HOME/groupsock/include \
##    $$LIVE_HOME/liveMedia/include
#    #$$LIVE_HOME/BasicUsageEnvironment $$LIVE_HOME/BasicUsageEnvironment/include \
#    #$$LIVE_HOME/UsageEnvironment $$LIVE_HOME/UsageEnvironment/include \
#    #$$LIVE_HOME/groupsock $$LIVE_HOME/groupsock/include \
#    #$$LIVE_HOME/liveMedia $$LIVE_HOME/liveMedia/include
##LIBS            += \
##    -L$$LIVE_HOME/BasicUsageEnvironment \
##    -L$$LIVE_HOME/UsageEnvironment \
##    -L$$LIVE_HOME/groupsock \
##    -L$$LIVE_HOME/liveMedia
#
#mac: INCLUDEPATH += $$SDK_HOME/usr/include

# EOF

#
# assistant.pro - Qt 4.7.3
#
#include(../../../shared/fontpanel/fontpanel.pri)
#TEMPLATE = app
#LANGUAGE = C++
#TARGET = assistant
#contains(QT_CONFIG, webkit):QT += webkit
#CONFIG += qt \
#    warn_on \
#    help
#QT += network
#PROJECTNAME = Assistant
#DESTDIR = ../../../../bin
#target.path = $$[QT_INSTALL_BINS]
#INSTALLS += target
#DEPENDPATH += ../shared
#
## ## Work around a qmake issue when statically linking to
## ## not-yet-installed plugins
#QMAKE_LIBDIR += $$QT_BUILD_TREE/plugins/sqldrivers
#HEADERS += aboutdialog.h \
#    bookmarkdialog.h \
#    bookmarkfiltermodel.h \
#    bookmarkitem.h \
#    bookmarkmanager.h \
#    bookmarkmanagerwidget.h \
#    bookmarkmodel.h \
#    centralwidget.h \
#    cmdlineparser.h \
#    contentwindow.h \
#    findwidget.h \
#    filternamedialog.h \
#    helpenginewrapper.h \
#    helpviewer.h \
#    indexwindow.h \
#    installdialog.h \
#    mainwindow.h \
#    preferencesdialog.h \
#    qtdocinstaller.h \
#    remotecontrol.h \
#    searchwidget.h \
#    topicchooser.h \
#    tracer.h \
#    xbelsupport.h \
#    ../shared/collectionconfiguration.h
#contains(QT_CONFIG, webkit) {
#    HEADERS += helpviewer_qwv.h
#} else {
#   HEADERS += helpviewer_qtb.h
# }
#win32:HEADERS += remotecontrol_win.h
#
#SOURCES += aboutdialog.cpp \
#    bookmarkdialog.cpp \
#    bookmarkfiltermodel.cpp \
#    bookmarkitem.cpp \
#    bookmarkmanager.cpp \
#    bookmarkmanagerwidget.cpp \
#    bookmarkmodel.cpp \
#    centralwidget.cpp \
#    cmdlineparser.cpp \
#    contentwindow.cpp \
#    findwidget.cpp \
#    filternamedialog.cpp \
#    helpenginewrapper.cpp \
#    helpviewer.cpp \
#    indexwindow.cpp \
#    installdialog.cpp \
#    main.cpp \
#    mainwindow.cpp \
#    preferencesdialog.cpp \
#    qtdocinstaller.cpp \
#    remotecontrol.cpp \
#    searchwidget.cpp \
#    topicchooser.cpp \
#    xbelsupport.cpp \
#    ../shared/collectionconfiguration.cpp
# contains(QT_CONFIG, webkit) {
#    SOURCES += helpviewer_qwv.cpp
#} else {
#    SOURCES += helpviewer_qtb.cpp
#}
#
#FORMS += bookmarkdialog.ui \
#    bookmarkmanagerwidget.ui \
#    bookmarkwidget.ui \
#    filternamedialog.ui \
#    installdialog.ui \
#    preferencesdialog.ui \
#    topicchooser.ui
#
#RESOURCES += assistant.qrc \
#    assistant_images.qrc
#
#win32 {
#    !wince*:LIBS += -lshell32
#    RC_FILE = assistant.rc
#}
#
#mac {
#    ICON = assistant.icns
#    TARGET = Assistant
#    QMAKE_INFO_PLIST = Info_mac.plist
#}
#
#contains(CONFIG, static): {
#    SQLPLUGINS = $$unique(sql-plugins)
#    contains(SQLPLUGINS, sqlite): {
#        QTPLUGIN += qsqlite
#        DEFINES += USE_STATIC_SQLITE_PLUGIN
#    }
#}
