# qtstep.pri
# 6/30/2011

INCLUDEPATH += $$PWD

HEADERS += $$PWD/qtstep.h

SOURCES += $$PWD/qtstep.mm

QT      += core gui
LIBS    += -framework Foundation -framework AppKit

# Carbon
# TO BE REPLACED WITH Cocoa
# See: http://developer.apple.com/library/mac/#documentation/MacOSX/Conceptual/BPFrameworks/Tasks/IncludingFrameworks.html
#DEFINES += USE_MAC_CARBON
#INCLUDEPATH += /Developer/Headers/FlatCarbon
#LIBS      += -framework Carbon

# EOF
