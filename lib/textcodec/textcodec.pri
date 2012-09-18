# textcodec.pri
# 6/16/2012

DEFINES += WITH_LIB_TEXTCODEC
DEPENDPATH += $$PWD

HEADERS += $$PWD/textcodec.h
SOURCES += \
    $$PWD/textcodec_ja.cc \
    $$PWD/textcodec_zh.cc

TABLE_FILES += \
    $$PWD/table/hirakata \
    $$PWD/table/zhszht \
    $$PWD/table/zhtzhs

OTHER_FILES += $$TABLE_FILES

QT += core

# EOF


