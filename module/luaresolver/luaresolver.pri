# luaresolver.pri
# 1/24/2012

include(../../config.pri)
include ($$ROOTDIR/module/luacpp/luacpp.pri)
include ($$ROOTDIR/module/download/download.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/luaresolver.h

SOURCES += \
    $$PWD/luaresolver.cc

OTHER_FILES += $$PWD/lua/luascript.lua

QT      += core network

# EOF
