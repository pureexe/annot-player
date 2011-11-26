# config.pri
# Since 9/3/2011

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
mac:    INCLUDEPATH     += $$ROOTDIR/mac
win32:  INCLUDEPATH     += $$ROOTDIR/win
unix:   INCLUDEPATH     += $$ROOTDIR/unix

## Compiling options

DEFINES += UNICODE

# C++0x
win32:  QMAKE_CXXFLAGS  += -Zc:auto
unix:   QMAKE_CXXFLAGS  += -std=c++0x
#mac:    QMAKE_CXXFLAGS  += -std=c++0x

#QMAKE_LFLAGS +=

## Deploy
# See: http://wiki.maemo.org/Packaging_a_Qt_application
# See: http://www.developer.nokia.com/Community/Wiki/Creating_Debian_packages_for_Maemo_5_Qt_applications_and_showing_in_the_application_menu
# See: https://wiki.kubuntu.org/PackagingGuide/QtApplication

unix {
  isEmpty(PREFIX): PREFIX = /usr
  BINDIR = $$PREFIX/bin
  DATADIR = $$PREFIX/share

  DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
}

## External libraries

win32 {
    VLC_HOME            = c:/dev/vlc
    #WSF_HOME            = c:/dev/wso2
    #CDIO_HOME          = c:/dev/cdio
    POPPLER_HOME        = c:/dev/poppler
    BOOST_HOME          = c:/dev/boost
    GSOAP_HOME          = c:/dev/gsoap
    ZLIB_HOME           = c:/dev/zlib

    ITH_HOME            = c:/dev/ith
    WDK_HOME            = c:/winddk

    INCLUDEPATH += c:/dev/inttypes/include

    #INCLUDEPATH        += $$ITH_HOME/include
    LIBS               += -L$$ITH_HOME/lib
    #INCLUDEPATH        += $$WDK_HOME/include
    LIBS               += -L$$WDK_HOME/lib
}

mac {
    VLC_HOME            = ${HOME}/opt/vlc
    #WSF_HOME            = ${HOME}/opt/wso2/wsf
    #CDIO_HOME          = ${HOME}/opt/libcdio
    POPPLER_HOME        = ${HOME}/opt/poppler
    BOOST_HOME          = ${HOME}/opt/local
    GSOAP_HOME          = ${HOME}/opt/local
    ZLIB_HOME           = /usr
}

unix {
    VLC_HOME            = /usr
    #WSF_HOME            = ${HOME}/opt/wso2/wsf
    #CDIO_HOME          = /usr
    POPPLER_HOME        = ${HOME}/opt/poppler
    BOOST_HOME          = /usr
    GSOAP_HOME          = /usr
    ZLIB_HOME           = /usr
}

INCLUDEPATH     += $$VLC_HOME/include
LIBS            += -L$$VLC_HOME/lib
#INCLUDEPATH     += $$WSF_HOME/include
#LIBS            += -L$$WSF_HOME/lib
#INCLUDEPATH     += $$CDIO_HOME/include
#LIBS            += -L$$CDIO_HOME/lib
INCLUDEPATH     += $$POPPLER_HOME/include/poppler/qt4
LIBS            += -L$$POPPLER_HOME/lib
INCLUDEPATH     += $$BOOST_HOME/include
LIBS            += -L$$BOOST_HOME/lib
INCLUDEPATH     += $$ZLIB_HOME/include
LIBS            += -L$$ZLIB_HOME/lib
INCLUDEPATH     += $$GSOAP_HOME/include
LIBS            += -L$$GSOAP_HOME/lib

CONFIG(release) {
    #DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
}

## DEBUG

include($$ROOTDIR/module/debug/debug.pri)

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
