# luacpp.pri
# 2/2/2012

include(../../config.pri)

DEFINES         += WITH_MODULE_LUACPP
DEPENDPATH      += $$PWD
HEADERS         += $$PWD/luacpp.h

LIBS    += -llua$$LUA_VERSION

# EOF
