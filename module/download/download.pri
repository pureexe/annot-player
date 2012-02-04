# download.pri
# 2/4/2012

include(../../config.pri)
include($$ROOTDIR/module/compress/compress.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/download.h \
    $$PWD/downloader.h

SOURCES += \
    $$PWD/download.cc \
    $$PWD/downloader.cc

QT      += core network

# EOF
