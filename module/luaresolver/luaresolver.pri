# luaresolver.pri
# 1/24/2012

include(../../config.pri)
include($$ROOTDIR/module/luatl/luatl.pri)

DEFINES += WITH_MODULE_LUARESOLVER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/luaresolver.h

SOURCES += \
    $$PWD/luaresolver.cc

LUA_FILES += \
    $$PWD/lua/luascript/add.lua \
    $$PWD/lua/luascript/customadd.lua \
    $$PWD/lua/luascript/lib/bit.lua \
    $$PWD/lua/luascript/lib/lalib.lua \
    $$PWD/lua/luascript/lib/login.lua \
    $$PWD/lua/luascript/sitelist/acfun.lua \
    $$PWD/lua/luascript/sitelist/bilibili.lua \
    $$PWD/lua/luascript/sitelist/mikufans.lua \
    $$PWD/lua/luascript/sitelist/nicovideo.lua \
    $$PWD/lua/luascript/sitelist/qqvideo.lua \
    $$PWD/lua/luascript/sitelist/sina.lua \
    $$PWD/lua/luascript/sitelist/tudou.lua \
    $$PWD/lua/luascript/sitelist/youku.lua \
    $$PWD/lua/luascript.lua

OTHER_FILES += $$LUA_FILES

QT      += core network

LIBS    += -llua$$LUA_VERSION

# EOF
