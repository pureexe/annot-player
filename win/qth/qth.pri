# qth.pri
# 10/13/2011

DEFINES += WITH_WIN_QTH

INCLUDEPATH += \
    $$PWD
DEPENDPATH += \
    $$PWD \
    $$PWD/ith

QT      += core
LIBS    += -lqth

HEADERS += \
    $$PWD/qth_config.h \
    $$PWD/qth.h

OTHER_FILES += \
    $$PWD/qth.pro

# EOF
