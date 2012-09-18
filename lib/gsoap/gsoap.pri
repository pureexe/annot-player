# gsoap.pri
# 9/10/2011
include(../../config.pri)

DEFINES += WITH_LIB_GSOAP

INCLUDEPATH += $$GSOAP_SRC/gsoap

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

#DEFINES += WITH_NONAMESPACES
#DEFINES += WITH_NOGLOBAL
DEFINES += WITH_COOKIES
DEFINES += WITH_GZIP HAVE_ZLIB_H # require zlib, see: http://www.cs.fsu.edu/~engelen/soapdoc2.html
DEFINES += WITH_SOAPDEFS_H  # include user-defined soapdefs.h

HEADERS += \
    $$PWD/soapdefs.h

SOURCES += \
    $$PWD/import_stdsoap2.cc \
    $$PWD/nsmap.cc \
    $$PWD/env/envC.cpp

OTHER_FILES += \
    $$PWD/env.gen.cmd \
    $$PWD/env.gen.sh

LIBS += -lz

# EOF
