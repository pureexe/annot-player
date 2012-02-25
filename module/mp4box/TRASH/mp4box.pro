# mp4box.pro
# 2/24/2012

include(../../config.pri)

CONFIG += noqt

TEMPLATE = lib

!mac: TARGET = mp4box
mac:  TARGET = MP4Box

LIBS    += -lgpac

# Use dylib instead of frameworks, which gave me lots of trouble orz
# see: http://doc.qt.nokia.com/latest/qmake-variable-reference.html#config
CONFIG  += shared
mac:    CONFIG += dylib lib_bundle
win32:  CONFIG += dll embed_manifest_dll

DEFINES += MP4BOX_SHARED

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

## Sources

HEADERS += \
    $$PWD/gpac/mp4box_config.h \
    $$PWD/gpac.h \
    $$PWD/mp4box.h

SOURCES += \
    $$PWD/gpac/mp4box_filedump.c \
    $$PWD/gpac/mp4box_fileimport.c \
    $$PWD/gpac/mp4box_live.c \
    $$PWD/gpac/mp4box_main.c

OTHER_FILES += \
    mp4box.pri \
    mp4box_shared.pri

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += mp4box.rc
}

# EOF
