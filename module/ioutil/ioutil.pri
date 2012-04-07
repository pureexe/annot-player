# ioutil.pri
# 12/6/2011

include(../../config.pri)

DEFINES += WITH_MODULE_IOUTIL

#win32:  include($$ROOTDIR/win/qtwin/qtwin.pri)
#unix:   include($$ROOTDIR/unix/qtunix/qtunix.pri)

INCLUDEPATH += $$PWD
HEADERS += $$PWD/ioutil.h
SOURCES += $$PWD/ioutil.cc

QT      += core

# EOF
