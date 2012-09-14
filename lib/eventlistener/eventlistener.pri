# eventlistener.pri
# 7/14/2012

DEFINES += WITH_LIB_EVENTLISTENER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/eventlistener.h \
    $$PWD/mouseclickeventlistener.h \
    $$PWD/mouseholdeventlistener.h

SOURCES += \
    $$PWD/mouseclickeventlistener.cc \
    $$PWD/mouseholdeventlistener.cc

QT      += core gui

# EOF
