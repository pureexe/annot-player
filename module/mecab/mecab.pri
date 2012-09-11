# mecab.pri
# 9/3/2012

DEFINES += WITH_MODULE_MECAB

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/mecabhighlighter.h \
    $$PWD/mecabparser.h \
    $$PWD/mecabsettings.h

SOURCES += \
    $$PWD/mecabhighlighter.cc \
    $$PWD/mecabparser.cc \
    $$PWD/mecabsettings.cc 

LIBS += -lmecab
QT += core gui

OTHER_FILES += $$PWD/etc/mecabrc

# EOF
