# clientdelegate.pri
# 10/6/2011

DEFINES += WITH_MODULE_CLIENTAGENT

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/clientsoap

HEADERS += \
    $$PWD/clientagent.h \
    $$PWD/clientdelegate.h \
    $$PWD/clientservice.h \
    $$PWD/clientservice_config.h \
    $$PWD/clientsoap.h

SOURCES += \
    $$PWD/clientagent.cc \
    $$PWD/clientservice.cc \
    $$PWD/clientsoap.cc

OTHER_FILES += \
    $$PWD/wsdl/clientagent.wsdl \
    $$PWD/wsdl/clientagent.xsd

QT      += core

# EOF
