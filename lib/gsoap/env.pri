# env.pri
# 10/7/2011

ENV_SOAP_HOME=$$PWD/env

INCLUDEPATH += \
  $$PWD \
  $$ENV_SOAP_HOME

HEADERS += \
    $$CLIENT_SOAP_HOME/env.h \
    $$CLIENT_SOAP_HOME/envH.h \
    $$CLIENT_SOAP_HOME/envStub.h

SOURCES += \
    $$CLIENT_SOAP_HOME/envC.cpp

OTHER_FILES += \
    $$PWD/env.gen.cmd \
    $$PWD/env.gen.sh

# EOF
