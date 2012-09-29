# sakurakit.pri
# 9/22/2012 jichi

DEFINES += WITH_LIB_SAKURAKIT
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/skstyleclass.h \
    $$PWD/skglobal.h \
    $$PWD/skquickview.h \
    $$PWD/sktoolbutton.h \
    $$PWD/skui.h

SOURCES += \
    $$PWD/skquickview.cc \
    $$PWD/skui.cc

QT += core gui qml quick widgets

# EOF
