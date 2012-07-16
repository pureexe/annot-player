# translator.pri
# 11/2/2011

DEFINES += WITH_MODULE_TRANSLATOR

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/bingtranslator.h \
    $$PWD/bingtranslator_p.h \
    $$PWD/googletranslator.h \
    $$PWD/googletranslator_p.h \
    $$PWD/microsofttranslator.h \
    $$PWD/microsofttranslator_p.h \
    $$PWD/romajitranslator.h \
    $$PWD/romajitranslator_p.h \
    $$PWD/translatormanager.h \
    $$PWD/translatorsettings.h \
    $$PWD/translator.h

SOURCES += \
    $$PWD/bingtranslator.cc \
    $$PWD/googletranslator.cc \
    $$PWD/microsofttranslator.cc \
    $$PWD/romajitranslator.cc \
    $$PWD/translatormanager.cc \
    $$PWD/translator.cc

QT      += core xml network webkit

# EOF
