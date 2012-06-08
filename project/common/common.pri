# common.pri
# 3/29/2012

DEFINES     += WITH_PROJECT_COMMON
DEPENDPATH  += $$PWD

HEADERS += \
    $$PWD/acabout.h \
    $$PWD/acaccountprefs_p.h \
    $$PWD/acapplication.h \
    $$PWD/acbrowser.h \
    $$PWD/accomboedit.h \
    $$PWD/accomboedit_p.h \
    $$PWD/acconsole.h \
    $$PWD/acdownloader.h \
    $$PWD/acfilteredlistview.h \
    $$PWD/acfilteredtableview.h \
    $$PWD/acglobal.h \
    $$PWD/achorizontalslider.h \
    $$PWD/aciconbutton.h \
    $$PWD/acipc.h \
    $$PWD/aclineedit.h \
    $$PWD/aclocationprefs_p.h \
    $$PWD/acmainwindow.h \
    $$PWD/acnetworkproxyprefs_p.h \
    $$PWD/acpaths.h \
    $$PWD/acplayer.h \
    $$PWD/acpreferences.h \
    $$PWD/acpreferencestab.h \
    $$PWD/acrc.h \
    $$PWD/acsettings.h \
    $$PWD/acss.h \
    $$PWD/acslider.h \
    $$PWD/actabview.h \
    $$PWD/actextedit.h \
    $$PWD/actextview.h \
    $$PWD/actimeedit.h \
    $$PWD/acui.h \
    $$PWD/acwebwindow.h \
    $$PWD/acwindow.h

SOURCES += \
    $$PWD/acabout.cc \
    $$PWD/acaccountprefs_p.cc \
    $$PWD/acapplication.cc \
    $$PWD/acbrowser.cc \
    $$PWD/accomboedit.cc \
    $$PWD/acconsole.cc \
    $$PWD/acdownloader.cc \
    $$PWD/acfilteredlistview.cc \
    $$PWD/acfilteredtableview.cc \
    $$PWD/achorizontalslider.cc \
    $$PWD/aciconbutton.cc \
    $$PWD/acipc.cc \
    $$PWD/aclineedit.cc \
    $$PWD/aclocationprefs_p.cc \
    $$PWD/acmainwindow.cc \
    $$PWD/acpaths.cc \
    $$PWD/acplayer.cc \
    $$PWD/acpreferences.cc \
    $$PWD/acnetworkproxyprefs_p.cc \
    $$PWD/acsettings.cc \
    $$PWD/acslider.cc \
    $$PWD/actabview.cc \
    $$PWD/actextedit.cc \
    $$PWD/actextview.cc \
    $$PWD/actimeedit.cc \
    $$PWD/acui.cc \
    $$PWD/acwebwindow.cc \
    $$PWD/acwindow.cc

win32:  SOURCES += $$PWD/acapplication_win.cc

RESOURCES += \
    $$PWD/common.qrc

QT      += core gui

# EOF
