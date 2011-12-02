# project.pro
# 7/31/2011

TEMPLATE = subdirs
SUBDIRS = \
    player
    #x
    #reader

win32: SUBDIRS += app

# EOF
