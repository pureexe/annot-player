# crypt.pri
# 6/28/2011

include(../../config.pri)

DEFINES += WITH_MODULE_CRYPT

HEADERS += \
    $$PWD/crypt.h \
    $$PWD/simplecrypt.h

SOURCES += \
    $$PWD/crypt.cc \
    $$PWD/simplecrypt.cc

QT +=   core

# EOF
