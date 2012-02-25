# webbrowser_static.pri
# 10/9/2011
include(../../config.pri)
INCLUDEPATH += $$PWD

DEFINES +=  WEBBROWSER_STATICLIB

FORMS   += \
    $$PWD/webbrowser.ui

HEADERS += \
    $$PWD/webbrowser_config.h \
    $$PWD/webbrowser.h \
    $$PWD/webbrowserprivate.h

SOURCES += \
    $$PWD/webbrowser.cc

RESOURCES += \
    $$PWD/webbrowser.qrc

QT += core gui network webkit
#CONFIG(static): QTPLUGIN += qgif qico qjpeg gsvg qtiff

#mac:  LIBS  += -framework WebBrowser

# EOF
