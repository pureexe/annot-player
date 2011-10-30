# webbrowser_static.pri
# Since 10/9/2011

include(../../config.pri)
INCLUDEPATH += $$PWD

DEFINES +=  WEBBROWSER_STATICLIB

FORMS   += \
    $$PWD/webbrowser.ui

HEADERS += \
    $$PWD/webbrowser_config.h \
    $$PWD/webbrowser.h

SOURCES += \
    $$PWD/webbrowser.cc

RESOURCES += \
    $$PWD/webbrowser.qrc

QT += core gui webkit

#mac:  LIBS  += -framework WebBrowser

# EOF
