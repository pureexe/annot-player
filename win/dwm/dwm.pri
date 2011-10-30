# dwm.pri
# Since 7/10/2011

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/dwm_config.h \
    $$PWD/dwm.h
SOURCES += \
    $$PWD/dwm.cc

CONFIG(dwmapi_static) {
    DEFINES += USE_STATIC_LIB
    LIBS += -Ldwmapi
}

QT      += core gui
LIBS    += -luser32

# EOF
