# translator.pri
# 11/2/2011

DEFINES += WITH_MODULE_TRANSLATOR

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/excitetranslator.h \
    $$PWD/excitetranslator_p.h \
    $$PWD/fresheyetranslator.h \
    $$PWD/fresheyetranslator_p.h \
    $$PWD/googletranslator.h \
    $$PWD/googletranslator_p.h \
    $$PWD/infoseektranslator.h \
    $$PWD/infoseektranslator_p.h \
    $$PWD/jdictranslator.h \
    $$PWD/jdictranslator_p.h \
    $$PWD/kotobanktranslator.h \
    $$PWD/kotobanktranslator_p.h \
    $$PWD/microsofttranslator.h \
    $$PWD/microsofttranslator_p.h \
    $$PWD/networktranslator.h \
    $$PWD/niftytranslator.h \
    $$PWD/niftytranslator_p.h \
    $$PWD/ocntranslator.h \
    $$PWD/ocntranslator_p.h \
    $$PWD/romajitranslator.h \
    $$PWD/romajitranslator_p.h \
    $$PWD/sdltranslator.h \
    $$PWD/sdltranslator_p.h \
    $$PWD/systrantranslator.h \
    $$PWD/systrantranslator_p.h \
    $$PWD/translatormanager.h \
    $$PWD/translatorsettings.h \
    $$PWD/translator.h \
    $$PWD/yahootranslator.h \
    $$PWD/yahootranslator_p.h

SOURCES += \
    $$PWD/excitetranslator.cc \
    $$PWD/fresheyetranslator.cc \
    $$PWD/googletranslator.cc \
    $$PWD/infoseektranslator.cc \
    $$PWD/jdictranslator.cc \
    $$PWD/kotobanktranslator.cc \
    $$PWD/microsofttranslator.cc \
    $$PWD/networktranslator.cc \
    $$PWD/niftytranslator.cc \
    $$PWD/ocntranslator.cc \
    $$PWD/romajitranslator.cc \
    $$PWD/sdltranslator.cc \
    $$PWD/systrantranslator.cc \
    $$PWD/translatormanager.cc \
    $$PWD/translator.cc \
    $$PWD/yahootranslator.cc

QT      += core xml network webkit

# EOF
