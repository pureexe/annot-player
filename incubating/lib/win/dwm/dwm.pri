# dwm.pri
# 7/10/2011 jichi

DEFINES += WITH_WIN_DWM
DEFINES += WITH_DWM_NOTIFIER

DEPENDPATH      += $$PWD

HEADERS += \
    $$PWD/dwm.h \
    $$PWD/dwmapi_p.h \
    $$PWD/dwmnotifier_p.h

SOURCES += \
    $$PWD/dwm.cc

CONFIG(dwmapi_static) {
    DEFINES += WITH_DWM_STATIC
    LIBS += -Ldwmapi
}

QT      += core gui
LIBS    += -luser32

# EOF
