# qtstep.pri
# 6/30/2011

DEFINES += WITH_MAC_QTSTEP

INCLUDEPATH     += $$PWD/..
DEPENDPATH      += $$PWD

HEADERS         += $$PWD/qtstep.h
OBJECTIVE_SOURCES += $$PWD/qtstep.mm

QT      += core gui
LIBS    += -framework Foundation -framework AppKit

# Carbon
# TO BE REPLACED WITH Cocoa
# See: http://developer.apple.com/library/mac/#documentation/MacOSX/Conceptual/BPFrameworks/Tasks/IncludingFrameworks.html
#DEFINES += USE_MAC_CARBON
#INCLUDEPATH += /Developer/Headers/FlatCarbon
#LIBS      += -framework Carbon

# EOF
