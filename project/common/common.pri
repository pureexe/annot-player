# common.pri
# 3/29/2012

DEFINES     += WITH_PROJECT_COMMON
DEPENDPATH  += $$PWD

HEADERS += \
    $$PWD/acaboutdialog.h \
    $$PWD/acbrowser.h \
    $$PWD/accomboedit.h \
    $$PWD/accomboedit_p.h \
    $$PWD/acdownloader.h \
    $$PWD/acfilteredlistview.h \
    $$PWD/acfilteredtableview.h \
    $$PWD/acglobal.h \
    $$PWD/acipc.h \
    $$PWD/aclineedit.h \
    $$PWD/acmainwindow.h \
    $$PWD/acpaths.h \
    $$PWD/acplayer.h \
    $$PWD/acrc.h \
    $$PWD/acsettings.h \
    $$PWD/acss.h \
    $$PWD/actextedit.h \
    $$PWD/actextview.h \
    $$PWD/acui.h \
    $$PWD/acwebwindow.h

SOURCES += \
    $$PWD/acaboutdialog.cc \
    $$PWD/acbrowser.cc \
    $$PWD/accomboedit.cc \
    $$PWD/acdownloader.cc \
    $$PWD/acfilteredlistview.cc \
    $$PWD/acfilteredtableview.cc \
    $$PWD/acipc.cc \
    $$PWD/aclineedit.cc \
    $$PWD/acmainwindow.cc \
    $$PWD/acpaths.cc \
    $$PWD/acplayer.cc \
    $$PWD/acsettings.cc \
    $$PWD/actextedit.cc \
    $$PWD/actextview.cc \
    $$PWD/acui.cc \
    $$PWD/acwebwindow.cc

RESOURCES += \
    $$PWD/common.qrc

QT      += core gui

# EOF
