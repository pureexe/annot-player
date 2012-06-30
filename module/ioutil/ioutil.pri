# ioutil.pri
# 12/6/2011

DEFINES += WITH_MODULE_IOUTIL

#win32:  include($$ROOTDIR/win/qtwin/qtwin.pri)
#unix:   include($$ROOTDIR/unix/qtunix/qtunix.pri)

DEPENDPATH += $$PWD
HEADERS += $$PWD/ioutil.h
SOURCES += $$PWD/ioutil.cc

QT      += core

# EOF
