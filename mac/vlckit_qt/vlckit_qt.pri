# vlckit_qt.pri
# Since 6/30/2011

include(../../config.pri)
include($$ROOTDIR/mac/VLCKit/VLCKit.pri)
include($$ROOTDIR/mac/qtstep/qtstep.pri)

HEADERS += \
    $$PWD/vlckit_qt.h \

SOURCES += \
   $$PWD/vlckit_qt.mm

QT      += core
LIBS    += -framework Foundation

# EOF
