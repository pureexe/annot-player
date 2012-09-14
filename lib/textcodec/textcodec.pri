# textcodec.pri
# 6/16/2012

DEFINES += WITH_LIB_TEXTCODEC
DEPENDPATH += $$PWD

HEADERS += $$PWD/textcodec.h
SOURCES += \
    $$PWD/textcodec_ja.cc \
    $$PWD/textcodec_zh.cc

TABLE_FILES += \
    $$ROOTDIR/lib/qtext/table/hirakata.txt \
    $$ROOTDIR/lib/qtext/table/zhszht.txt \
    $$ROOTDIR/lib/qtext/table/zhtzhs.txt

OTHER_FILES += $$TABLE_FILES

QT += core

# EOF


