# blockiodevice.pri
# 12/3/2011

include(../../config.pri)

win32: INCLUDEPATH += $$PWD/wdk

INCLUDEPATH += $$PWD
HEADERS += $$PWD/blockiodevice.h
SOURCES += $$PWD/blockiodevice.cc

QT      += core

# EOF
