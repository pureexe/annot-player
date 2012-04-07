# nicoutil.pri
# 3/14/2012

include(../../config.pri)

DEFINES += WITH_MODULE_NICOUTIL

INCLUDEPATH += $$PWD
HEADERS += $$PWD/nicoutil.h
SOURCES += $$PWD/nicoutil.cc

QT      += core network

# EOF
