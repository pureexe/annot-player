# annotcloud.pri
# 6/28/2011

include(../../config.pri)

HEADERS += \
    $$PWD/alias.h\
    $$PWD/annotationparser.h \
    $$PWD/annotation.h \
    $$PWD/cmd.h \
    $$PWD/traits.h \
    $$PWD/token.h \
    $$PWD/user.h

SOURCES += \
    $$PWD/alias.cc \
    $$PWD/annotationparser.cc \
    $$PWD/annotation.cc \
    $$PWD/token.cc \
    $$PWD/user.cc

QT +=   core xml webkit

# EOF


