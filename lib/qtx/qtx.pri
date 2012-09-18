# qtx.pri
# 6/28/2011

DEFINES += WITH_LIB_QTX
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/qtxglobal.h \
    $$PWD/qxactionwithid.h \
    $$PWD/qxalgorithm.h \
    $$PWD/qxbitwise.h \
    $$PWD/qxbytearray.h \
    $$PWD/qxcheckbox.h \
    $$PWD/qxcombobox.h \
    $$PWD/qxcompactstackedlayout.h \
    $$PWD/qxcountdowntimer.h \
    $$PWD/qxdateedit.h \
    $$PWD/qxdatetime.h \
    $$PWD/qxdatetimeedit.h \
    $$PWD/qxdebug.h \
    $$PWD/qxdoublespinbox.h \
    $$PWD/qxdialog.h \
    $$PWD/qxdraggablemainwindow.h \
    $$PWD/qxdraggablewidget.h \
    $$PWD/qxeventforwarder.h \
    $$PWD/qxfiledeleter.h \
    $$PWD/qxfontcombobox.h \
    $$PWD/qxfs.h \
    $$PWD/qxhash.h \
    $$PWD/qxhighlighttextedit.h \
    $$PWD/qxlayoutwidget.h \
    $$PWD/qxnamedcheckbox.h \
    $$PWD/qxnamedradiobutton.h \
    $$PWD/qxnetwork.h \
    $$PWD/qxnetworkcookie.h \
    $$PWD/qxos.h \
    $$PWD/qxoverlaylayout.h \
    $$PWD/qxradiobutton.h \
    $$PWD/qxrubberband.h \
    $$PWD/qxslider.h \
    $$PWD/qxspinbox.h \
    $$PWD/qxstoppable.h \
    $$PWD/qxstring.h \
    $$PWD/qxsystemtrayicon.h \
    $$PWD/qxtexthighlighter.h \
    $$PWD/qxtextedit.h \
    $$PWD/qxtimeedit.h \
    $$PWD/qxtoolbutton.h \
    $$PWD/qxtoolbuttonwithid.h \
    $$PWD/qxwebview.h \
    $$PWD/qxwebpage.h \
    $$PWD/qxwebpluginfactory.h \
    $$PWD/qxwithsizehint.h

SOURCES += \
    $$PWD/qxactionwithid.cc \
    $$PWD/qxcheckbox.cc \
    $$PWD/qxcombobox.cc \
    $$PWD/qxcountdowntimer.cc \
    $$PWD/qxdateedit.cc \
    $$PWD/qxdatetime.cc \
    $$PWD/qxdatetimeedit.cc \
    $$PWD/qxdoublespinbox.cc \
    $$PWD/qxdraggablemainwindow.cc \
    $$PWD/qxdraggablewidget.cc \
    $$PWD/qxfiledeleter.cc \
    $$PWD/qxfontcombobox.cc \
    $$PWD/qxfs.cc \
    $$PWD/qxhighlighttextedit.cc \
    $$PWD/qxnetworkcookie.cc \
    $$PWD/qxos.cc \
    $$PWD/qxradiobutton.cc \
    $$PWD/qxrubberband.cc \
    $$PWD/qxslider.cc \
    $$PWD/qxspinbox.cc \
    $$PWD/qxstring.cc \
    $$PWD/qxtexthighlighter.cc \
    $$PWD/qxtextedit.cc \
    $$PWD/qxtimeedit.cc \
    $$PWD/qxtoolbutton.cc \
    $$PWD/qxtoolbuttonwithid.cc \
    $$PWD/qxwebview.cc \
    $$PWD/qxwebpage.cc \
    $$PWD/qxwebpluginfactory.cc

QT +=   core gui network webkit

IMAGE_FILES += \
    $$PWD/images/favicon.ico \
    $$PWD/images/null.png

OTHER_FILES += \
    $$IMAGE_FILES

# EOF
