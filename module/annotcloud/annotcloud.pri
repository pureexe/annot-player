# annotcloud.pri
# 6/28/2011

include(../../config.pri)

HEADERS += \
    $$PWD/alias.h\
    $$PWD/annotation.h \
    $$PWD/annothtml.h \
    $$PWD/annottag.h \
    $$PWD/annotxml.h \
    $$PWD/traits.h \
    $$PWD/token.h \
    $$PWD/user.h

SOURCES += \
    $$PWD/alias.cc \
    $$PWD/annotation.cc \
    $$PWD/annothtml.cc \
    $$PWD/annothtmlparse.cc \
    $$PWD/annothtmlthread.cc \
    $$PWD/annothtmlunparse.cc \
    $$PWD/annotxml.cc \
    $$PWD/token.cc \
    $$PWD/user.cc

QT +=   core xml webkit

# EOF


