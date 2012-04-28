# mrlanalysis.pri
# 4/11/2012

include(../../config.pri)

DEFINES += WITH_MODULE_MRLANALYSIS

DEPENDPATH += $$PWD

HEADERS += $$PWD/mrlanalysis.h
SOURCES += $$PWD/mrlanalysis.cc

QT      += core

# EOF
