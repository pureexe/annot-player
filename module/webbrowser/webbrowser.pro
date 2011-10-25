# webbrowser.pro
# Since 7/10/2011
# See: http://bezstellar.blog.163.com/blog/static/1748687802010101844811643/
# Original since: 27. jul 14:00:10 2009, by qmake (2.01a) ma
include(../../config.pri)

QT += core gui webkit

TEMPLATE = lib
TARGET = webbrowser

# see: http://doc.qt.nokia.com/latest/qmake-variable-reference.html#config
CONFIG  += shared
win32:  CONFIG += dll embed_manifest_dll
mac:    CONFIG += dylib lib_bundle

DEFINES += WEBBROWSER_LIB

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

## Sources

FORMS = webbrowser.ui

HEADERS = \
    webbrowser.h \
    webbrowser_config.h

SOURCES = \
    webbrowser.cc

OTHER_FILES = \
    webbrowser.rc \
    webbrowser.pri

RESOURCES += webbrowser.qrc

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += webbrowser.rc
}

# EOF
