# downloatask.pri
# 2/4/2012

DEFINES += WITH_LIB_DOWNLOADTASK

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/downloadmanager.h \
    $$PWD/downloadtask.h \
    $$PWD/mrldownloadtask.h

SOURCES += \
    $$PWD/downloadmanager.cc \
    $$PWD/downloadtask.cc \
    $$PWD/mrldownloadtask.cc \
    $$PWD/mrldownloadtask_m.cc \
    $$PWD/mrldownloadtask_s.cc

QT      += core network

# EOF
