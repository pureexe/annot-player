# annot.pri
# 9/3/2011

## Config

ROOTDIR = $$PWD
BUILDDIR = $$ROOTDIR/../annot-build-desktop

win32:  DESTDIR         = $$BUILDDIR/build.win
unix:   DESTDIR         = $$BUILDDIR/build.unix
mac:    DESTDIR         = $$BUILDDIR/build.mac
win32:  DESTDIR_TARGET  = $$BUILDDIR/release.win
unix:   DESTDIR_TARGET  = $$BUILDDIR/release.unix
mac:    DESTDIR_TARGET  = $$BUILDDIR/release.mac

LIBS            += -L$$DESTDIR
mac:  LIBS      += -F$$DESTDIR
INCLUDEPATH     += $$ROOTDIR
win32:  INCLUDEPATH     += $$ROOTDIR/win
unix:   INCLUDEPATH     += $$ROOTDIR/unix
mac:    INCLUDEPATH     += $$ROOTDIR/mac

## Compiling options

mac:    CONFIG -= ppc ppc64

DEFINES += UNICODE

#CONFIG += x86 x86_64 ppc64

# C++0x
win32:  QMAKE_CXXFLAGS += -Zc:auto
unix:   QMAKE_CXXFLAGS += -std=c++0x
mac:    QMAKE_CXXFLAGS -= -std=c++0x

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

## Deploy
# See: http://wiki.maemo.org/Packaging_a_Qt_application
# See: http://www.developer.nokia.com/Community/Wiki/Creating_Debian_packages_for_Maemo_5_Qt_applications_and_showing_in_the_application_menu
# See: https://wiki.kubuntu.org/PackagingGuide/QtApplication

LUADIR=$$DESTDIR/lua
DOCDIR=$$DESTDIR/doc
IMAGEDIR=$$DESTDIR/images
JSFDIR=$$DESTDIR/jsf

unix:!mac {
    isEmpty(PREFIX): PREFIX = /usr
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
}
mac {
    isEmpty(PREFIX): PREFIX = /opt/annot
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
}

!win32 {
  DEFINES += \
    BINDIR=\\\"$$BINDIR\\\" \
    DATADIR=\\\"$$DATADIR\\\"
}
DEFINES += \
    DOCDIR=\\\"$$DOCDIR\\\" \
    IMAGEDIR=\\\"$$IMAGDIR\\\" \
    LUADIR=\\\"$$LUADIR\\\" \
    JSFDIR=\\\"$$JSFDIR\\\"

## External libraries

win32 {
    QT_HOME             = c:/qt/current
    QT_SRC              = $$QT_HOME/src
    #VLC_HOME            = "c:/Program Files/VideoLAN/VLC/sdk"
    VLC_HOME            = c:/dev/vlc
    VLC_SRC             = $$VLC_HOME/src
    #WSF_HOME            = c:/dev/wso2
    #CDIO_HOME          = c:/dev/cdio
    #FFMPEG_HOME         = c:/dev/ffmpeg
    GPAC_HOME           = c:/dev/gpac
    MP4V2_HOME          = c:/dev/mp4v2
    LIVE_HOME           = c:/dev/live
    POPPLER_HOME        = c:/dev/poppler
    BOOST_HOME          = c:/dev/boost
    GSOAP_HOME          = c:/dev/gsoap
    ZLIB_HOME           = c:/dev/zlib
    LUA_HOME            = c:/dev/lua
    LUA_VERSION =

    ITH_HOME            = c:/dev/ith
    WDK_HOME            = c:/winddk

    INCLUDEPATH += c:/dev/inttypes/include

    #INCLUDEPATH        += $$ITH_HOME/include
    LIBS               += -L$$ITH_HOME/lib

    # wdk/inc/api/sal.h MUST be removed.
    # See:  http://stackoverflow.com/questions/1356653/multiple-compiling-errors-with-basic-c-application-on-vs2010-beta-1
    INCLUDEPATH        += $$WDK_HOME/include
    LIBS               += -L$$WDK_HOME/lib
}

unix {
    QT_HOME             = /usr/share/qt4
    QT_SRC              =
    VLC_HOME            = /usr
    VLC_SRC             = ${HOME}/opt/src
    #WSF_HOME            = ${HOME}/opt/wso2/wsf
    #CDIO_HOME          = /usr
    #FFMPEG_HOME         = /usr
    GPAC_HOME           = /usr
    MP4V2_HOME          = /usr
    LIVE_HOME           = /usr
    POPPLER_HOME        = ${HOME}/opt/poppler
    BOOST_HOME          = /usr
    GSOAP_HOME          = /usr
    LUA_HOME            = /usr
    ZLIB_HOME           = /usr
    LUA_VERSION = 5.1
}

mac {
    QT_HOME             = /opt/local/share/qt4
    QT_SRC              = /Developer/QtSDK/QtSources/4.7.4/src
    #VLC_HOME            = ${HOME}/opt/vlc
    VLC_HOME            = /Applications/VLC.app/Contents/MacOS
    VLC_SRC             = ${HOME}/opt/src
    #WSF_HOME           = ${HOME}/opt/wso2/wsf
    #CDIO_HOME          = ${HOME}/opt/libcdio
    #FFMPEG_HOME         = /opt/local
    GPAC_HOME           = ${HOME}/opt/gpac
    MP4V2_HOME          = /opt/local
    LIVE_HOME           = ${HOME}/opt/live
    POPPLER_HOME        = ${HOME}/opt/poppler
    BOOST_HOME          = /opt/local
    GSOAP_HOME          = /opt/local
    ZLIB_HOME           = /usr
    LUA_HOME            = /usr
    LUA_VERSION =
}

INCLUDEPATH     += $$VLC_HOME/include
INCLUDEPATH     += $$VLC_HOME/include/vlc/plugins
#INCLUDEPATH     += $$VLC_SRC/include
LIBS            += -L$$VLC_HOME/lib
#INCLUDEPATH     += $$WSF_HOME/include
#LIBS            += -L$$WSF_HOME/lib
#INCLUDEPATH     += $$CDIO_HOME/include
#LIBS            += -L$$CDIO_HOME/lib
#INCLUDEPATH     += $$POPPLER_HOME/include/poppler/qt4
#LIBS            += -L$$POPPLER_HOME/lib
INCLUDEPATH     += $$BOOST_HOME/include
LIBS            += -L$$BOOST_HOME/lib
INCLUDEPATH     += $$GSOAP_HOME/include
LIBS            += -L$$GSOAP_HOME/lib
INCLUDEPATH     += $$LUA_HOME/include \
                   $$LUA_HOME/include/lua$$LUA_VERSION
LIBS            += -L$$LUA_HOME/lib
INCLUDEPATH     += $$ZLIB_HOME/include
LIBS            += -L$$ZLIB_HOME/lib
#INCLUDEPATH     += $$FFMPEG_HOME/include
#LIBS            += -L$$FFMPEG_HOME/lib
INCLUDEPATH     += $$GPAC_HOME/include
LIBS            += -L$$GPAC_HOME/lib
INCLUDEPATH     += $$MP4V2_HOME/include
LIBS            += -L$$MP4V2_HOME/lib
#INCLUDEPATH     += \
#    $$LIVE_HOME/BasicUsageEnvironment/include \
#    $$LIVE_HOME/UsageEnvironment/include \
#    $$LIVE_HOME/groupsock/include \
#    $$LIVE_HOME/liveMedia/include
    #$$LIVE_HOME/BasicUsageEnvironment $$LIVE_HOME/BasicUsageEnvironment/include \
    #$$LIVE_HOME/UsageEnvironment $$LIVE_HOME/UsageEnvironment/include \
    #$$LIVE_HOME/groupsock $$LIVE_HOME/groupsock/include \
    #$$LIVE_HOME/liveMedia $$LIVE_HOME/liveMedia/include
#LIBS            += \
#    -L$$LIVE_HOME/BasicUsageEnvironment \
#    -L$$LIVE_HOME/UsageEnvironment \
#    -L$$LIVE_HOME/groupsock \
#    -L$$LIVE_HOME/liveMedia

CONFIG(release) {
  #DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
}

CONFIG(noqt) {
  CONFIG -= qt
  QT     -= core gui
  LIBS   -= -lQtCore -lQtGui
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

include($$ROOTDIR/module/debug/debug.pri)

## Deploy

DEFINES += VERSION=\\\"$$VERSION\\\"

## Domains and accounts

DEFINES += NICOVIDEO_USERNAME=\\\"oedivocin@gmail.com\\\" \
           NICOVIDEO_PASSWORD=\\\"nicovideo.jp\\\"

#DEFINES += ANNOT_PROXY_DOMAIN=\\\"sakuradite.com\\\"
DEFINES += ANNOT_PROXY_DOMAIN=\\\"210.175.52.140\\\"
DEFINES += ANNOT_HOST_IP=\\\"210.175.54.32\\\" \
           ANNOT_DOC_IP=\\\"210.175.52.140\\\"

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
