# mrlresolver.pri
# 1/24/2012

include(../../config.pri)
include ($$ROOTDIR/module/luacpp/luacpp.pri)
include ($$ROOTDIR/module/download/download.pri)

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/lua
DEPENDPATH += $$PWD/lua

HEADERS += \
    $$PWD/luamrlresolver.h \
    $$PWD/googlevideomrlresolver.h \
    $$PWD/mediainfo.h \
    $$PWD/mrlresolver.h \
    $$PWD/mrlresolvermanager.h \
    $$PWD/youkumrlresolver.h \
    $$PWD/youtubemrlresolver.h \
    $$PWD/lua/luascript.h

SOURCES += \
    $$PWD/luamrlresolver.cc \
    $$PWD/googlevideomrlresolver.cc \
    $$PWD/mediainfo.cc \
    $$PWD/mrlresolvermanager.cc \
    $$PWD/youkumrlresolver.cc \
    $$PWD/youtubemrlresolver.cc \
    $$PWD/lua/luascript.cc

OTHER_FILES += $$PWD/lua/luascript.lua

QT      += core network script

# EOF
