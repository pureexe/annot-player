# tr.pri
# 9/16/2011

include(../../../config.pri)

HEADERS += \
    tr.h \
    translationmanager.h

SOURCES += \
    translationmanager.cc

TRANSLATIONS += \
    $$PWD/tr_en.ts \
    $$PWD/tr_ja.ts \
    $$PWD/tr_zh_TW.ts \
    $$PWD/tr_zh_CN.ts

OTHER_FILES += $$TRANSLATIONS \
    $$PWD/lrelease_tr.cmd \
    $$PWD/lrelease_tr.sh \
    $$PWD/lupdate_tr.cmd \
    $$PWD/lupdate_tr.sh

QT += core

# EOF
