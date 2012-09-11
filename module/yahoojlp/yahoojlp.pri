# yahoojlp.pri
# 9/8/2012

DEFINES += WITH_MODULE_YAHOOJLP

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/yahoojlpapis_p.h \
    $$PWD/yahoojlphighlighter.h
    #$$PWD/yahoojlpparser.h

SOURCES += \
    $$PWD/yahoojlphighlighter.cc
    #$$PWD/yahoojlpparser.cc

QT += core gui network xml

# EOF
