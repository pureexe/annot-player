# compress.pri
# 2/3/2012

DEFINES += WITH_MODULE_COMPRESS
DEFINES += WITH_GZIP

DEPENDPATH      += $$PWD

HEADERS += $$PWD/qgzip.h
SOURCES += $$PWD/qgzip.cc

QT      += core
LIBS    += -lz

# EOF
