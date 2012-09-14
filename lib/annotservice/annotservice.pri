# annotservice.pri
# 9/10/2011

DEFINES += WITH_LIB_ANNOTSERVICE

INCLUDEPATH += \
    $$PWD \
    $$PWD/cloudservice
DEPENDPATH += \
    $$PWD \
    $$PWD/cloudservice

HEADERS += \
    $$PWD/annotserveragent.h \
    $$PWD/annotserverproxy_p.h \
    $$PWD/castmanager_p.h \
    $$PWD/cloudservice_config.h \
    $$PWD/cloudservicesoap_p.h

SOURCES += \
    $$PWD/annotserveragent.cc \
    $$PWD/annotserverproxy_p.cc \
    $$PWD/castmanager_p.cc \
    $$PWD/cloudservicesoap_p.cc

QT      += core
OTHER_FILES += \
    $$PWD/serversoap.update.cmd \
    $$PWD/serversoap.update.sh
