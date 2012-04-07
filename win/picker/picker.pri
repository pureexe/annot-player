# picker.pri
# 10/30/2011

INCLUDEPATH += $$PWD

DEFINES += WITH_WIN_PICKER

HEADERS += \
    $$PWD/picker_config.h \
    $$PWD/picker.h

QT      += core gui

LIBS    += -lpicker

OTHER_FILES += \
    $$PWD/picker.pro

# EOF
