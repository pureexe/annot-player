# qth.pri
# 10/13/2011

DEPENDPATH  += $$PWD
INCLUDEPATH += $$PWD

DEFINES += WITH_WIN_QTH

QT      += core

LIBS    += -lqth

HEADERS += \
    $$PWD/qth_config.h \
    $$PWD/qth.h

OTHER_FILES += \
    $$PWD/qth.pro

# EOF
