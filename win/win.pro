# win.pro
# 7/23/2011
win32 {

TEMPLATE = subdirs
#include(dwm/dwm.pri)
#include(qtwin/qtwin.pri)
#include(qtwinnt/qtwinnt.pri)
#include(wmtimer/wmtimer.pri)
#SUBDIRS += hook
SUBDIRS += mousehook
SUBDIRS += picker
#SUBDIRS += texthook

}
# EOF
