# vlchttp.pri
# 6/28/2011

DEFINES += WITH_MODULE_VLCHTTP

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/httpplugin.h \
    $$PWD/httpbufferedsession_p.h \
    $$PWD/httpsession_p.h \
    $$PWD/httpstreamsession_p.h

SOURCES += \
    $$PWD/httpplugin.cc \
    $$PWD/httpbufferedsession_p.cc \
    $$PWD/httpsession_p.cc \
    $$PWD/httpstreamsession_p.cc

#DEFINES += HAVE_ZLIB_H HAVE_STRUCT_ADDRINFO

QT      += core gui network
LIBS    += -lvlc -lvlccore

INCLUDEPATH += \
    $$VLC_SRC \
    $$VLC_SRC/vlc/include

# EOF
