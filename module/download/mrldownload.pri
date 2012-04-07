# mrldownload.pri
# 2/4/2012

include(../../config.pri)

DEFINES += WITH_MODULE_MRLDOWNLOAD

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mrldownloadtask.h

SOURCES += \
    $$PWD/mrldownloadtask.cc

QT      += core network

# EOF
