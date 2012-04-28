# picker.pri
# 10/30/2011

INCLUDEPATH     += $$PWD
DEPENDPATH      += $$PWD

DEFINES += WITH_WIN_PICKER

HEADERS += \
    $$PWD/picker_config.h \
    $$PWD/picker.h

OTHER_FILES += \
    $$PWD/picker.pro

QT      += core gui
LIBS    += -lpicker

# EOF
