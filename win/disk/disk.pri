# disk.pri
# 8/17/2011

INCLUDEPATH += $$PWD/ddk

HEADERS += \
    $$PWD/disk_config.h \
    $$PWD/disk.h

SOURCES += \
    $$PWD/disk.cc

QT      += core
LIBS    += -luser32

# EOF
