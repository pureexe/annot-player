# mousehook.pri
# 11/26/2011

INCLUDEPATH += $$PWD

DEFINES += WITH_WIN_MOUSEHOOK

HEADERS += \
    $$PWD/mousehook_config.h \
    $$PWD/mousehook.h

QT      += core gui

LIBS    += -lmousehook

OTHER_FILES += \
    $$PWD/mousehook.pro

# EOF
