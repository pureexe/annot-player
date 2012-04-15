# common.pri
# 3/29/2012

INCLUDEPATH += \
    $$PWD/include
DEPENDPATH += \
    $$PWD/include \
    $$PWD/include/ac

HEADERS += \
    $$PWD/include/ac/acbrowser.h \
    $$PWD/include/ac/accomboedit.h \
    $$PWD/include/ac/acdownloader.h \
    $$PWD/include/ac/acfilteredlistview.h \
    $$PWD/include/ac/acfilteredtableview.h \
    $$PWD/include/ac/acglobal.h \
    $$PWD/include/ac/acipc.h \
    $$PWD/include/ac/aclineedit.h \
    $$PWD/include/ac/acmainwindow.h \
    $$PWD/include/ac/acplayer.h \
    $$PWD/include/ac/acrc.h \
    $$PWD/include/ac/acsettings.h \
    $$PWD/include/ac/acss.h \
    $$PWD/include/ac/actextedit.h \
    $$PWD/include/ac/actextview.h \
    $$PWD/include/ac/acui.h \
    $$PWD/include/ac/acwebwindow.h \
    $$PWD/include/accomboedit_p.h

SOURCES += \
    $$PWD/src/acbrowser.cc \
    $$PWD/src/accomboedit.cc \
    $$PWD/src/acdownloader.cc \
    $$PWD/src/acfilteredlistview.cc \
    $$PWD/src/acfilteredtableview.cc \
    $$PWD/src/acipc.cc \
    $$PWD/src/aclineedit.cc \
    $$PWD/src/acmainwindow.cc \
    $$PWD/src/acplayer.cc \
    $$PWD/src/acsettings.cc \
    $$PWD/src/actextedit.cc \
    $$PWD/src/actextview.cc \
    $$PWD/src/acui.cc \
    $$PWD/src/acwebwindow.cc

RESOURCES += $$PWD/common.qrc

QT      += core gui

# EOF
