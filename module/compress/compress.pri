# annotcodec.pri
# 2/3/2012

include(../../config.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qgzip.h

SOURCES += \
    $$PWD/qgzip.cc

QT      += core
LIBS    += -lz

# EOF
