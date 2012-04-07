# hook.pri
# 7/20/2011

INCLUDEPATH += $$PWD

DEFINES += WITH_WIN_HOOK

HEADERS += \
    $$PWD/hook_config.h \
    $$PWD/hook.h

DEFINES += USE_MOUSE_HOOK

QT      += core gui

LIBS    += -lhook

OTHER_FILES += \
    $$PWD/hook.pro

# EOF
