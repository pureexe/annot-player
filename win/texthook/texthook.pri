# texthook.pri
# 10/13/2011

DEFINES += WITH_WIN_TEXTHOOK

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT      += core
LIBS    += -ltexthook

HEADERS += \
    $$PWD/texthook_config.h \
    $$PWD/texthook.h

OTHER_FILES += \
    $$PWD/texthook.pro

# EOF
