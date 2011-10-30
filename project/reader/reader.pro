# reader.pro
# Since 2011-06-18T21:54:19

include (../../config.pri)

## Libraries
include($$ROOTDIR/core/core.pri)
include(core/core.pri)

win32 {
    include($$ROOTDIR/win/dwm/dwm.pri)
    DEFINES += USE_DWM
}

QT      += core gui webkit network

## Sources

TARGET      = reader
TEMPLATE    = app

HEADERS += \
    annotationwidget.h \
    catalogwidget.h \
    documentwidget.h \
    mainwindow.h \
    maintabwidget.h \
    pagewidget.h \
    previewwidget.h \
    renderthread.h \
    stylesheet.h \
    thumbnailwidget.h

SOURCES += \
    annotationwidget.cc \
    catalogwidget.cc \
    documentwidget.cc \
    main.cc \
    maintabwidget.cc \
    mainwindow.cc \
    pagewidget.cc \
    previewwidget.cc \
    renderthread.cc \
    thumbnailwidget.cc

FORMS   += mainwindow.ui

RESOURCES += reader.qrc

OTHER_FILES += \
    Info.plist \
    reader.rc \
    reader.ico \
    reader.icns

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += reader.rc
}

mac {
    ICON = reader.icns
    QMAKE_INFO_PLIST = Info.plist
}


# EOF
