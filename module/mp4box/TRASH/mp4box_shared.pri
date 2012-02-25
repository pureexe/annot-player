# mp4box_shared.pri
# 2/15/2012

include(../../config.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD/gpac

HEADERS += \
    $$PWD/gpac/mp4box_config.h \
    $$PWD/gpac.h \
    $$PWD/mp4box.h

SOURCES += \
    $$PWD/mp4box.cc

OTHER_FILES += \
    $$PWD/mp4box.pri \
    $$PWD/mp4box.pro

QT      += core
!mac: LIBS += -lmp4box
mac:  LIBS += -framework MP4Box

# EOF
