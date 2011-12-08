# ioutil.pri
# 12/6/2011

include(../../config.pri)

#win32:  include($$ROOTDIR/win/qtwin/qtwin.pri)
#unix:   include($$ROOTDIR/unix/qtunix/qtunix.pri)
include($$ROOTDIR/module/blockiodevice/blockiodevice.pri)
DEFINES += USE_MODULE_BLOCKIODEVICE

INCLUDEPATH += $$PWD
HEADERS += $$PWD/ioutil.h
SOURCES += $$PWD/ioutil.cc

QT      += core

# EOF
