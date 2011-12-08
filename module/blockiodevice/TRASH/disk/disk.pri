# disk.pri
# 8/17/2011

include(../../config.pri)

INCLUDEPATH += $$PWD/ddk

HEADERS += $$PWD/disk.h
SOURCES += $$PWD/disk.cc

QT      += core
LIBS    += -luser32

# EOF
