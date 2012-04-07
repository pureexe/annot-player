# doll.pri
# 6/31/2011

include(../../config.pri)
include($$ROOTDIR/module/aiml/aiml.pri)

DEFINES += WITH_MODULE_DOLL

INCLUDEPATH     += $$PWD
QT              += core

HEADERS += \
    $$PWD/doll_config.h \
    $$PWD/doll.h \
    $$PWD/doll_tr.h

SOURCES += \
    $$PWD/doll.cc


# EOF
