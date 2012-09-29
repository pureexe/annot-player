# reader.pro
# 9/22/2012 jichi

include(../../config.pri)

## Include
include($$ROOTDIR/lib/portable/sakurakit/sakurakit.pri)
include($$ROOTDIR/lib/portable/qx/qx.pri)

win32 {
  include($$ROOTDIR/lib/win/dwm/dwm.pri)
  include($$ROOTDIR/lib/win/qtwin/qtwin.pri)
  include($$ROOTDIR/lib/win/reg/reg.pri)
}

## Sources

TEMPLATE = app

QT += core gui qml quick widgets

TARGET_PATH = \
    src \
    src/dialog \
    src/main

INCLUDEPATH += $$TARGET_PATH
DEPENDPATH += $$TARGET_PATH

HEADERS += \
    src/main/application.h \
    src/main/global.h \
    src/main/mainobject.h \
    src/main/mainobject_p.h \
    src/main/mainwidget.h \
    #src/main/mainwindow.h \
    src/main/qml.h \
    src/dialog/librarydialog.h

SOURCES += \
    src/main/application.cc \
    src/main/main.cc \
    src/main/mainobject.cc \
    src/main/mainobject_p.cc \
    src/main/mainwidget.cc \
    #src/main/mainwindow.cc \
    src/dialog/librarydialog.cc

OTHER_FILES += TODO

QML_FILES += qml

INSTALLS += qml
qml.path = qml
qml.files = $$QML_FILES

# EOF
