# qt.pri
# 4/6/2012

DEFINES += WITH_LIB_QT

#RESOURCES += $$PWD/qt.qrc
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/qtrc.h \
    $$PWD/qtsettings.h

SOURCES += \
    $$PWD/qtsettings.cc

DEFINES += QT_HOME=\\\"$$QT_HOME\\\"

INCLUDEPATH     += $$QT_SRC

#TRANSLATIONS += \
#    $$PWD/tr/tr_en.ts \
#    $$PWD/tr/tr_ja.ts \
#    $$PWD/tr/tr_zh_CN.ts \
#    $$PWD/tr/tr_zh_TW.ts

# EOF
