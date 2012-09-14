# annotdown.pri
# y/6/2012

DEFINES += WITH_LIB_ANNOTDOWN

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/annotationdownloader.h

SOURCES += \
    $$PWD/annotationdownloader.cc 

QT      += core network

# EOF
