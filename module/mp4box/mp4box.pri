# mp4box.pri
# 2/15/2012

DEFINES += WITH_MODULE_MP4BOX

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
DEPENDPATH += $$PWD/gpac

#DEFINES += MP4BOX_STATIC

HEADERS += $$PWD/mp4box.h
SOURCES += $$PWD/mp4box.cc
QT      += core

!win32 {
  DEFINES += WITH_GPAC
  LIBS    += -lgpac

  DEPENDPATH += $$PWD/gpac
  HEADERS += \
    $$PWD/gpac/gpac.h \
    $$PWD/gpac/mp4box_config.h

  SOURCES += \
    $$PWD/gpac/mp4box_filedump.c \
    $$PWD/gpac/mp4box_fileimport.c \
    $$PWD/gpac/mp4box_live.c \
    $$PWD/gpac/mp4box_main.c
}

# EOF
