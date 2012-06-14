# luatl.pri
# 2/2/2012
# LUA template library for C++

include(../../config.pri)

DEFINES         += WITH_MODULE_LUATL
DEPENDPATH      += $$PWD
HEADERS         += \
    $$PWD/lua_defines.h \
    $$PWD/lua_function.h

LIBS    += -llua$$LUA_VERSION

# EOF
