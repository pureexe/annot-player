# core.pri
# Since 6/28/2011

include(../config.pri)

HEADERS += \
    $$PWD/annotationparser.h \
    $$PWD/cmd.h \
    $$PWD/eventlistener.h \
    $$PWD/htmltag.h \
    $$PWD/macro.h \
    $$PWD/os.h \
    $$PWD/ss.h \
    $$PWD/data/annotation.h \
    $$PWD/data/traits.h \
    $$PWD/data/alias.h \
    $$PWD/data/token.h \
    $$PWD/data/user.h \
    $$PWD/gui/actionwithid.h \
    $$PWD/gui/combobox.h \
    $$PWD/gui/dialog.h \
    $$PWD/gui/fontcombobox.h \
    $$PWD/gui/slider.h \
    $$PWD/gui/toolbutton.h \
    $$PWD/gui/webdialog.h \
    $$PWD/util/codec.h \
    $$PWD/util/crypt.h \
    $$PWD/util/datetime.h \
    $$PWD/util/hash.h

SOURCES += \
    $$PWD/annotationparser.cc \
    $$PWD/os.cc \
    $$PWD/data/annotation.cc \
    $$PWD/data/token.cc \
    $$PWD/data/user.cc \
    $$PWD/gui/actionwithid.cc \
    $$PWD/gui/combobox.cc \
    $$PWD/gui/dialog.cc \
    $$PWD/gui/fontcombobox.cc \
    $$PWD/gui/slider.cc \
    $$PWD/gui/toolbutton.cc \
    $$PWD/gui/webdialog.cc \
    $$PWD/util/codec.cc \
    $$PWD/util/crypt.cc \
    $$PWD/util/datetime.cc

QT      += core gui xml network webkit

# TODO: Create a generate disk class!! not only for win32
win32:  include($$ROOTDIR/win/disk/disk.pri)

# EOF


