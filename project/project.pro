# project.pro
# 7/31/2011

TEMPLATE = subdirs
SUBDIRS = \
    player
    #reader

win32:  SUBDIRS += app
#unix:   SUBDIRS += x

# EOF
