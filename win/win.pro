# win.pro
# 7/23/2011
win32 {

TEMPLATE = subdirs
#include(disk/disk.pri)
include(dwm/dwm.pri)
include(qtwin/qtwin.pri)
#SUBDIRS += hook
SUBDIRS += mousehook
SUBDIRS += picker
SUBDIRS += qth
#SUBDIRS += qtith

}
# EOF
