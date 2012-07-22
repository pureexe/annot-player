# vlccocoa.pri
# 6/30/2011

DEFINES += WITH_MAC_VLCCOCOA

include(../../config.pri)
#include($$ROOTDIR/mac/qtcocoa/qtcocoa.pri)
#include($$PWD/minimal_macosx/minimal_macosx.pri)

# Static link mostly because framework would complicate deployment on mac
# Esp. it also required VLC plugins.
# See: http://doc.qt.nokia.com/stable/deployment-mac.html
#include($$ROOTDIR/mac/VLCKit/VLCKit_shared.pri)
include($$ROOTDIR/mac/VLCKit/VLCKit_static.pri)

INCLUDEPATH     += $$PWD/..
DEPENDPATH      += $$PWD

HEADERS         +=  $$PWD/vlccocoa.h
OBJECTIVE_HEADERS += $$PWD/VLCOpenGLVoutView_public.h
OBJECTIVE_SOURCES +=  $$PWD/vlccocoa.mm

QT      += core
LIBS    += -framework Foundation

LIBS        += -lvlccore

INCLUDEPATH += \
    $$VLC_SRC \
    $$VLC_SRC/vlc/modules/gui/minimal_macosx

# EOF
