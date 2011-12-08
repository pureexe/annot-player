# env.pri
# 10/7/2011

## TODO: Automatic generate this pri file!!

ENV_SOAP_HOME=$$PWD/env

INCLUDEPATH += $$PWD
INCLUDEPATH += $$SOAP_PREFIX

HEADERS += \
    $$CLIENT_SOAP_HOME/env.h \
    $$CLIENT_SOAP_HOME/envH.h \
    $$CLIENT_SOAP_HOME/envStub.h

SOURCES += \
    $$CLIENT_SOAP_HOME/envC.cpp

OTHER_FILES += \
    $$PWD/env.gen.cmd

# EOF
