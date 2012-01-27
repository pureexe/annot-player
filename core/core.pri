# core.pri
# 6/28/2011

include(../config.pri)

HEADERS += \
    $$PWD/annotationparser.h \
    $$PWD/cmd.h \
    $$PWD/eventlistener.h \
    $$PWD/htmltag.h \
    $$PWD/macro.h \
    $$PWD/ss.h \
    $$PWD/cloud/alias.h \
    $$PWD/cloud/annotation.h \
    $$PWD/cloud/traits.h \
    $$PWD/cloud/token.h \
    $$PWD/cloud/user.h \
    $$PWD/gui/actionwithid.h \
    $$PWD/gui/combobox.h \
    $$PWD/gui/dialog.h \
    $$PWD/gui/fontcombobox.h \
    $$PWD/gui/slider.h \
    $$PWD/gui/toolbutton.h \
    $$PWD/gui/webdialog.h \
    $$PWD/gui/withsizehint.h \
    $$PWD/util/textcodec.h \
    $$PWD/util/countdowntimer.h \
    $$PWD/util/crypt.h \
    $$PWD/util/datetime.h \
    $$PWD/util/hash.h

SOURCES += \
    $$PWD/annotationparser.cc \
    $$PWD/cloud/alias.cc \
    $$PWD/cloud/annotation.cc \
    $$PWD/cloud/token.cc \
    $$PWD/cloud/user.cc \
    $$PWD/gui/actionwithid.cc \
    $$PWD/gui/combobox.cc \
    $$PWD/gui/dialog.cc \
    $$PWD/gui/fontcombobox.cc \
    $$PWD/gui/slider.cc \
    $$PWD/gui/toolbutton.cc \
    $$PWD/gui/webdialog.cc \
    $$PWD/util/textcodec.cc \
    $$PWD/util/countdowntimer.cc \
    $$PWD/util/crypt.cc \
    $$PWD/util/datetime.cc

QT +=   core gui xml network webkit

# EOF


