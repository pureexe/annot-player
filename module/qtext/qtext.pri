# qtext.pri
# 6/28/2011

include(../../config.pri)

DEFINES += WITH_MODULE_QTEXT
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/actionwithid.h \
    $$PWD/algorithm.h \
    $$PWD/bitwise.h \
    $$PWD/combobox.h \
    $$PWD/compactstackedlayout.h \
    $$PWD/countdowntimer.h \
    $$PWD/datetime.h \
    $$PWD/dialog.h \
    $$PWD/draggablemainwindow.h \
    $$PWD/draggablewidget.h \
    $$PWD/eventlistener.h \
    $$PWD/filedeleter.h \
    $$PWD/filesystem.h \
    $$PWD/fontcombobox.h \
    $$PWD/hash.h \
    $$PWD/htmltag.h \
    $$PWD/layoutwidget.h \
    $$PWD/network.h \
    $$PWD/networkcookie.h \
    $$PWD/os.h \
    $$PWD/overlaylayout.h \
    $$PWD/rubberband.h \
    $$PWD/slider.h \
    $$PWD/stoppable.h \
    $$PWD/string.h \
    $$PWD/ss.h \
    $$PWD/systemtrayicon.h \
    $$PWD/toolbutton.h \
    $$PWD/toolbuttonwithid.h \
    $$PWD/webview.h \
    $$PWD/webpage.h \
    $$PWD/webpluginfactory.h \
    $$PWD/withsizehint.h

SOURCES += \
    $$PWD/actionwithid.cc \
    $$PWD/combobox.cc \
    $$PWD/countdowntimer.cc \
    $$PWD/datetime.cc \
    $$PWD/dialog.cc \
    $$PWD/draggablemainwindow.cc \
    $$PWD/draggablewidget.cc \
    $$PWD/filedeleter.cc \
    $$PWD/filesystem.cc \
    $$PWD/fontcombobox.cc \
    $$PWD/networkcookie.cc \
    $$PWD/os.cc \
    $$PWD/rubberband.cc \
    $$PWD/slider.cc \
    $$PWD/string.cc \
    $$PWD/toolbutton.cc \
    $$PWD/toolbuttonwithid.cc \
    $$PWD/webview.cc \
    $$PWD/webpage.cc \
    $$PWD/webpluginfactory.cc

QT +=   core gui network webkit

DOC_FILES += \
    $$PWD/doc/error.html \
    $$PWD/doc/start.html

IMAGE_FILES += \
    $$PWD/images/favicon.ico \
    $$PWD/images/null.png

OTHER_FILES += \
    $$DOC_FILES \
    $$IMAGE_FILES

# EOF


