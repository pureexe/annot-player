# common.pri
# 3/29/2012

INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/ac/accomboedit.h \
    $$PWD/include/ac/acfilteredlistview.h \
    $$PWD/include/ac/acfilteredtableview.h \
    $$PWD/include/ac/acglobal.h \
    $$PWD/include/ac/aclineedit.h \
    $$PWD/include/ac/acr.h \
    $$PWD/include/ac/acsettings.h \
    $$PWD/include/ac/acss.h \
    $$PWD/include/ac/actextedit.h \
    $$PWD/include/ac/actextview.h \
    $$PWD/include/ac/acui.h \
    $$PWD/include/accomboeditprivate.h

SOURCES += \
    $$PWD/src/accomboedit.cc \
    $$PWD/src/acfilteredlistview.cc \
    $$PWD/src/acfilteredtableview.cc \
    $$PWD/src/aclineedit.cc \
    $$PWD/src/acsettings.cc \
    $$PWD/src/actextedit.cc \
    $$PWD/src/actextview.cc \
    $$PWD/src/acui.cc

RESOURCES += $$PWD/common.qrc

QT      += core gui

# EOF
