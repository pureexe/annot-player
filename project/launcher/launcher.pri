# launcher.pri
# 3/29/2012

DEFINES     += WITH_PROJECT_LAUNCHER
INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/include

HEADERS += \
    $$PWD/include/main.tcc \
    $$PWD/include/msc.h

QT      += core

# EOF
