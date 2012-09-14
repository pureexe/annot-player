# annotcache.pri
# 6/27/2012

DEFINES += WITH_LIB_ANNOTCACHE

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/annotationcachemanager.h 

SOURCES += \
    $$PWD/annotationcachemanager.cc 

QT      += core network

# EOF
