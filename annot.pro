# annot.pro
# 7/23/2011

TEMPLATE = subdirs

include(core/core.pri)
SUBDIRS = \
    project \
    module
win32:  SUBDIRS += win
unix:   SUBDIRS += unix
mac:    SUBDIRS += mac

OTHER_FILES = \
  ChangeLog \
  COPYING \
  README \
  TODO \
  licenses/annot-player.txt

# EOF
