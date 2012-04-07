# serveragent.pri
# 9/10/2011

include(../../config.pri)

DEFINES += WITH_MODULE_SERVERAGENT

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/serversoap

HEADERS += \
    $$PWD/castmanager.h \
    $$PWD/serveragent.h \
    $$PWD/serverproxy.h \
    $$PWD/serverproxy_config.h \
    $$PWD/serversoap.h

SOURCES += \
    $$PWD/castmanager.cc \
    $$PWD/serveragent.cc \
    $$PWD/serverproxy.cc \
    $$PWD/serversoap.cc

QT      += core
OTHER_FILES += \
    $$PWD/serversoap.update.cmd

# EOF
