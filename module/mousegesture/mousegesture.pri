# mousegesture.pri
# 4/25/2012
# See: http://doc.qt.nokia.com/qq/qq18-mousegestures.html

include(../../config.pri)

DEFINES += WITH_MODULE_MOUSEGESTURE

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/mousegesture.h \
    $$PWD/mousegesturefilter.h \
    $$PWD/mousegesturefilter_p.h

SOURCES += \
    $$PWD/mousegesture.cc \
    $$PWD/mousegesturefilter.cc \
    $$PWD/mousegesturefilter_p.cc

QT      += core gui

# EOF
