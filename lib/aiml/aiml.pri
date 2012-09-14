# aiml.pri
# 6/31/2011

DEFINES += WITH_LIB_AIML

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/aiml_config.h \
    $$PWD/aiml.h \
    $$PWD/aimlbot.h \
    $$PWD/aimlparser.h

SOURCES += \
    $$PWD/aimlbot.cc \
    $$PWD/aimlparser.cc

QT      += core gui xml

# TO BE MOVED TO win32/qtwin
win32: LIBS += advapi32.lib

# EOF
