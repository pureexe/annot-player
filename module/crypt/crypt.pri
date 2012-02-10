# crypt.pri
# 6/28/2011

include(../../config.pri)

HEADERS += \
    $$PWD/crypt.h \
    $$PWD/simplecrypt.h

SOURCES += \
    $$PWD/crypt.cc \
    $$PWD/simplecrypt.cc

QT +=   core

# EOF
