# translator.pri
# 11/2/2011

DEFINES += WITH_MODULE_TRANSLATOR

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/translator_config.h \
    $$PWD/translator.h

SOURCES += \
    $$PWD/translator.cc

QT      += core network xml

# EOF
