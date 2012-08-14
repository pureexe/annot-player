# annot.pro
# 7/23/2011

TEMPLATE = subdirs

SUBDIRS = \
    project
    #module
win32:  SUBDIRS += win
#unix:   SUBDIRS += unix
#mac:    SUBDIRS += mac

OTHER_FILES = \
    licenses/annot-browser.txt \
    licenses/annot-down.txt \
    licenses/annot-player.txt \
    scripts/"Reset Preferences.reg" \
    ChangeLog \
    COPYING \
    README \
    TODO \
    UPDATE

# EOF
