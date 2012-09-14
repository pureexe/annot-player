# mrlresolver.pri
# 1/24/2012

include(../../config.pri)
include($$ROOTDIR/lib/luaresolver/luaresolver.pri)
include($$ROOTDIR/lib/mrlanalysis/mrlanalysis.pri)

DEFINES += WITH_LIB_MRLRESOLVER

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dummygooglevideomrlresolver.h \
    $$PWD/dummyyoutubemrlresolver.h \
    $$PWD/luamrlresolver.h \
    $$PWD/mrlinfo.h \
    $$PWD/mrlresolver.h \
    $$PWD/mrlresolvermanager.h \
    $$PWD/mrlresolversettings.h \
    $$PWD/youkumrlresolver.h \
    $$PWD/youtubemrlresolver.h

SOURCES += \
    $$PWD/dummygooglevideomrlresolver.cc \
    $$PWD/dummyyoutubemrlresolver.cc \
    $$PWD/luamrlresolver.cc \
    $$PWD/mrlinfo.cc \
    $$PWD/mrlresolvermanager.cc \
    $$PWD/youkumrlresolver.cc \
    $$PWD/youtubemrlresolver.cc

QT      += core network script

# EOF
