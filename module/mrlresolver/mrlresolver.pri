# mrlresolver.pri
# 1/24/2012

include(../../config.pri)
include ($$ROOTDIR/module/luaresolver/luaresolver.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/luamrlresolver.h \
    $$PWD/googlevideomrlresolver.h \
    $$PWD/mrlinfo.h \
    $$PWD/mrlresolver.h \
    $$PWD/mrlresolvermanager.h \
    $$PWD/mrlresolversettings.h \
    $$PWD/youkumrlresolver.h \
    $$PWD/youtubemrlresolver.h

SOURCES += \
    $$PWD/luamrlresolver.cc \
    $$PWD/googlevideomrlresolver.cc \
    $$PWD/mrlinfo.cc \
    $$PWD/mrlresolvermanager.cc \
    $$PWD/youkumrlresolver.cc \
    $$PWD/youtubemrlresolver.cc

QT      += core network script

# EOF
