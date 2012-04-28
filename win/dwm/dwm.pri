# dwm.pri
# 7/10/2011

DEFINES += WITH_WIN_DWM
DEFINES += WITH_DWM_NOTIFIER

INCLUDEPATH     += $$PWD/..
DEPENDPATH      += $$PWD

HEADERS += \
    $$PWD/dwm.h \
    $$PWD/dwmapi.h \
    $$PWD/dwmnotifier.h

SOURCES += \
    $$PWD/dwm.cc

CONFIG(dwmapi_static) {
    DEFINES += WITH_DWM_STATIC
    LIBS += -Ldwmapi
}

QT      += core gui
LIBS    += -luser32

# EOF
