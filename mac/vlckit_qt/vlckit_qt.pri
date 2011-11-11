# vlckit_qt.pri
# Since 6/30/2011

include(../../config.pri)
include($$ROOTDIR/mac/qtstep/qtstep.pri)

# Static link mostly because framework would complicate deployment on mac
# Esp. it also required VLC plugins.
# See: http://doc.qt.nokia.com/stable/deployment-mac.html
#include($$ROOTDIR/mac/VLCKit/VLCKit_shared.pri)
include($$ROOTDIR/mac/VLCKit/VLCKit_static.pri)

HEADERS += \
    $$PWD/vlckit_qt.h \

SOURCES += \
   $$PWD/vlckit_qt.mm

QT      += core
LIBS    += -framework Foundation

# EOF
