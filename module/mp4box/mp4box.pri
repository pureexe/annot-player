# mp4box.pri
# 2/15/2012

include(../../config.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD/gpac

HEADERS += \
    $$PWD/gpac/mp4box_config.h \
    $$PWD/gpac.h \
    $$PWD/mp4box.h

SOURCES += \
    $$PWD/gpac/mp4box_filedump.c \
    $$PWD/gpac/mp4box_fileimport.c \
    $$PWD/gpac/mp4box_live.c \
    $$PWD/gpac/mp4box_main.c \
    $$PWD/mp4box.cc

QT      += core
LIBS    += -lgpac

# EOF
