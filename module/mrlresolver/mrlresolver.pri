# mrlresolver.pri
# 1/24/2012

include(../../config.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mrlresolver.h \
    $$PWD/mrlresolvermanager.h \
    $$PWD/youkumrlresolver.h \
    $$PWD/youtubemrlresolver.h

SOURCES += \
    $$PWD/mrlresolvermanager.cc \
    $$PWD/youkumrlresolver.cc \
    $$PWD/youtubemrlresolver.cc

QT      += core network script

# EOF
