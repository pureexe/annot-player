# qtcocoa.pri
# 6/30/2011

DEFINES += WITH_MAC_QTCOCOA

INCLUDEPATH     += $$PWD/..
DEPENDPATH      += $$PWD

HEADERS         += $$PWD/qtcocoa.h
OBJECTIVE_SOURCES += $$PWD/qtcocoa.mm

QT      += core gui
LIBS    += -framework Foundation -framework AppKit

# Carbon
# See: http://developer.apple.com/library/mac/#documentation/MacOSX/Conceptual/BPFrameworks/Tasks/IncludingFrameworks.html
#DEFINES += USE_MAC_CARBON
#INCLUDEPATH += /Developer/Headers/FlatCarbon
#LIBS      += -framework Carbon

# EOF
