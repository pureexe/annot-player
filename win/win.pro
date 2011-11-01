# win.pro
# Since 7/23/2011

TEMPLATE = subdirs
include(disk/disk.pri)
include(dwm/dwm.pri)
include(qtwin/qtwin.pri)
SUBDIRS += hook
SUBDIRS += picker
SUBDIRS += qth
#SUBDIRS += qtith

# EOF
