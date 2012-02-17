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

LUA_SCRIPTS += \
    $$PWD/lua/luascript/add.lua \
    $$PWD/lua/luascript/customadd.lua \
    $$PWD/lua/luascript/lib/bit.lua \
    $$PWD/lua/luascript/lib/lalib.lua \
    $$PWD/lua/luascript/lib/login.lua \
    $$PWD/lua/luascript/loginconfig.lua \
    $$PWD/lua/luascript/sitelist/acfun.lua \
    $$PWD/lua/luascript/sitelist/bilibili.lua \
    $$PWD/lua/luascript/sitelist/mikufans.lua \
    $$PWD/lua/luascript/sitelist/nicovideo.lua \
    $$PWD/lua/luascript/sitelist/qqvideo.lua \
    $$PWD/lua/luascript/sitelist/sina.lua \
    $$PWD/lua/luascript/sitelist/tudou.lua \
    $$PWD/lua/luascript/sitelist/youku.lua \
    $$PWD/lua/luascript.lua

OTHER_FILES += $$LUA_SCRIPTS

QT      += core network

# EOF
