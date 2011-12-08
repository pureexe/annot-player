# qtvlc.pri
# 6/30/2011

include(../../config.pri)
include($$ROOTDIR/mac/qtstep/qtstep.pri)
#include($$PWD/minimal_macosx/minimal_macosx.pri)

# Static link mostly because framework would complicate deployment on mac
# Esp. it also required VLC plugins.
# See: http://doc.qt.nokia.com/stable/deployment-mac.html
#include($$ROOTDIR/mac/VLCKit/VLCKit_shared.pri)
include($$ROOTDIR/mac/VLCKit/VLCKit_static.pri)

HEADERS += \
    $$PWD/qtvlc.h \
    $$PWD/qtvlcprivate.h

SOURCES += \
   $$PWD/qtvlc.mm

QT      += core
LIBS    += -framework Foundation

DEFINES     += WITH_VLCCORE
LIBS        += -lvlccore
INCLUDEPATH += $$VLC_SRC/vlc/modules/gui/minimal_macosx

# EOF
