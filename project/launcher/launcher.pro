# launcher.pro
# 3/29/2012

TEMPLATE = subdirs
SUBDIRS = \
    browser \
    downloader \
    player \
    translator \
    updater

OTHER_FILES += \
    include/msc.h \
    include/main.tcc

# EOF
