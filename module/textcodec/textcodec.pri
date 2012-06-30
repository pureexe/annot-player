# textcodec.pri
# 6/16/2012

DEFINES += WITH_MODULE_TEXTCODEC
DEPENDPATH += $$PWD

HEADERS += $$PWD/textcodec.h
SOURCES += $$PWD/textcodec.cc

DOC_FILES += \
  $$ROOTDIR/module/qtext/doc/utftable.zhs_zht.txt \
  $$ROOTDIR/module/qtext/doc/utftable.zht_zhs.txt

OTHER_FILES += $$DOC_FILES

QT += core

# EOF


