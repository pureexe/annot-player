# compress.pri
# 2/3/2012

include(../../config.pri)

DEFINES += WITH_MODULE_COMPRESS
DEFINES += WITH_GZIP

DEPENDPATH      += $$PWD

HEADERS += $$PWD/qgzip.h
SOURCES += $$PWD/qgzip.cc

QT      += core
LIBS    += -lz

# EOF
