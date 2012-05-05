# searchengine.pri
# 5/4/2012
include(../../config.pri)

DEFINES += WITH_MODULE_SEARCHENGINE

DEPENDPATH += \
    $$PWD

HEADERS += \
    $$PWD/searchengine.h \
    $$PWD/searchenginefactory.h \
    $$PWD/searchenginerc.h

SOURCES += \
    $$PWD/searchenginefactory.cc

RESOURCES += \
    $$PWD/searchengine.qrc

QT += core

# EOF
