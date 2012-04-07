# webbrowser_static.pri
# 10/9/2011
include(../../config.pri)

DEFINES += WITH_MODULE_WEBBROWSER

INCLUDEPATH += \
    $$PWD \
    $$PWD/gui \
    $$PWD/network \
    $$PWD/webkit
DEPENDPATH += \
    $$PWD \
    $$PWD/gui \
    $$PWD/network \
    $$PWD/webkit

DEFINES +=  WEBBROWSER_STATIC

FORMS   += \
    $$PWD/webbrowser.ui

HEADERS += \
    $$PWD/gui/wbaddresscomboedit.h \
    $$PWD/gui/wbcomboedit.h \
    $$PWD/gui/wblineedit.h \
    $$PWD/network/wbnetworkaccessmanager.h \
    $$PWD/network/wbnetworkcookiejar.h \
    $$PWD/webkit/wbwebview.h \
    $$PWD/wbrc.h \
    $$PWD/wbss.h \
    $$PWD/webbrowser.h \
    $$PWD/webbrowserprivate.h \
    $$PWD/webbrowser_config.h

SOURCES += \
    $$PWD/gui/wbaddresscomboedit.cc \
    $$PWD/gui/wbcomboedit.cc \
    $$PWD/gui/wblineedit.cc \
    $$PWD/network/wbnetworkaccessmanager.cc \
    $$PWD/webbrowser.cc

RESOURCES += \
    $$PWD/webbrowser.qrc

QT += core gui network webkit
#CONFIG(static): QTPLUGIN += qgif qico qjpeg gsvg qtiff

#mac:  LIBS  += -framework WebBrowser

# EOF
