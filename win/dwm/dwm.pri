# dwm.pri
# 7/10/2011

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/dwm_config.h \
    $$PWD/dwm.h
SOURCES += \
    $$PWD/dwm.cc

DEFINES += USE_DWM_NOTIFIER

CONFIG(dwmapi_static) {
    DEFINES += USE_DWM_STATIC
    LIBS += -Ldwmapi
}

QT      += core gui
LIBS    += -luser32

# EOF
