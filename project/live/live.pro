# live.pro
# 2/11/2011
include(../../config.pri)

TEMPLATE = app
mac: CONFIG -= app_bundle
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

LIBS   += \
     -lliveMedia \
     -lgroupsock \
     -lUsageEnvironment \
     -lBasicUsageEnvironment

SOURCES += main.cc

QT += core

# EOF
