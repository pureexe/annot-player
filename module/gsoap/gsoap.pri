# gsoap.pri
# Since 9/10/2011

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/env

#DEFINES += WITH_NONAMESPACES
#DEFINES += WITH_NOGLOBAL
DEFINES += WITH_COOKIES
DEFINES += WITH_GZIP HAVE_ZLIB_H # require zlib, see: http://www.cs.fsu.edu/~engelen/soapdoc2.html
DEFINES += WITH_SOAPDEFS_H  # include user-defined soapdefs.h

HEADERS += \
    $$PWD/soapdefs.h \
    $$PWD/stdsoap2.h

SOURCES += \
    $$PWD/stdsoap2.cpp \
    $$PWD/nsmap.cc \
    $$PWD/env/envC.cpp

LIBS += -lz

# EOF
