# mecabsettings.pri
# 9/3/2012

DEFINES += WITH_MODULE_MECABSETTINGS

DEPENDPATH += $$PWD

HEADERS += $$PWD/mecabsettings.h
SOURCES += $$PWD/mecabsettings.cc

LIBS += -lmecab
QT += core gui

OTHER_FILES += $$PWD/etc/mecabrc

# EOF
