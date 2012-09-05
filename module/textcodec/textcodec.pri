# textcodec.pri
# 6/16/2012

DEFINES += WITH_MODULE_TEXTCODEC
DEPENDPATH += $$PWD

HEADERS += $$PWD/textcodec.h
SOURCES += \
    $$PWD/textcodec_ja.cc \
    $$PWD/textcodec_zh.cc

TABLE_FILES += \
    $$ROOTDIR/module/qtext/table/hirakata.txt \
    $$ROOTDIR/module/qtext/table/zhszht.txt \
    $$ROOTDIR/module/qtext/table/zhtzhs.txt

OTHER_FILES += $$TABLE_FILES

QT += core

# EOF


