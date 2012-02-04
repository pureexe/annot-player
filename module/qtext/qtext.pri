# qtext.pri
# 6/28/2011

include(../../config.pri)

INCLUDEPATH += $$PWD/..

HEADERS += \
    $$PWD/actionwithid.h \
    $$PWD/combobox.h \
    $$PWD/countdowntimer.h \
    $$PWD/crypt.h \
    $$PWD/datetime.h \
    $$PWD/dialog.h \
    $$PWD/eventlistener.h \
    $$PWD/cloud/alias.h \
    $$PWD/fontcombobox.h \
    $$PWD/hash.h \
    $$PWD/htmltag.h \
    $$PWD/ss.h \
    $$PWD/slider.h \
    $$PWD/toolbutton.h \
    $$PWD/webdialog.h \
    $$PWD/withsizehint.h

SOURCES += \
    $$PWD/actionwithid.cc \
    $$PWD/combobox.cc \
    $$PWD/countdowntimer.cc \
    $$PWD/crypt.cc \
    $$PWD/datetime.cc \
    $$PWD/dialog.cc \
    $$PWD/fontcombobox.cc \
    $$PWD/slider.cc \
    $$PWD/toolbutton.cc \
    $$PWD/webdialog.cc

QT +=   core gui

# EOF


