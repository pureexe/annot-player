# applocale.pri
# 9/1/2012

DEFINES += WITH_WIN_APPLOCALE

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

HEADERS += $$PWD/applocale.h
SOURCES += $$PWD/applocale.cc 

QT +=   core

# EOF
