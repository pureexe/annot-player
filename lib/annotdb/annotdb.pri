# annotdb.pri
# 6/5/2012

DEFINES += WITH_LIB_ANNOTDB

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/annotdb.h \
    $$PWD/annotdbdefs.h 

SOURCES += \
    $$PWD/annotdb.cc 

QT      += core sql

# EOF
