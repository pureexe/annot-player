# vlchttp.pri
# 6/28/2011

DEFINES += WITH_MODULE_VLCHTTP

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/httpplugin.h \
    $$PWD/httpbufferedsession.h \
    $$PWD/httpsession.h \
    $$PWD/httpstreamsession.h

SOURCES += \
    $$PWD/httpplugin.cc \
    $$PWD/httpbufferedsession.cc \
    $$PWD/httpsession.cc \
    $$PWD/httpstreamsession.cc

#DEFINES += HAVE_ZLIB_H HAVE_STRUCT_ADDRINFO

QT      += core gui network
LIBS    += -lvlc -lvlccore

INCLUDEPATH += \
    $$VLC_SRC \
    $$VLC_SRC/vlc/include

# EOF
