# VLCKit.pro
# 10/26/2011
include(../../config.pri)

CONFIG      -= qt
QT          -= core gui
LIBS        -= -lQtCore -lQtGui

TEMPLATE    = lib
#TARGET      = VLCKit
TARGET      = vlckit

# Use dylib instead of frameworks, which gave me lots of trouble orz
# see: http://doc.qt.nokia.com/latest/qmake-variable-reference.html#config
CONFIG      += shared dylib #lib_bundle

#DEFINES     += VLCKIT_LIB

LIBS    += -lvlc
LIBS    += \
    -framework Foundation \
    -framework AppKit \
    -framework Quartz

## Sources

INCLUDEPATH += ./..

INCLUDEPATH += .
DEPENDPATH  += .
INCLUDEPATH += Internal
DEPENDPATH  += Internal
INCLUDEPATH += Config
DEPENDPATH  += Config

OBJECTIVE_HEADERS += \
    VLCAudio.h \
    VLCKit.h \
    VLCLibrary.h \
    VLCMedia.h \
    VLCMediaDiscoverer.h \
    VLCMediaLibrary.h \
    VLCMediaList.h \
    VLCMediaListPlayer.h \
    VLCMediaPlayer.h \
    VLCMediaThumbnailer.h \
    VLCServicesDiscoverer.h \
    VLCStreamOutput.h \
    VLCStreamSession.h \
    VLCTime.h \
    VLCVideoLayer.h \
    VLCVideoView.h \
    Config/VLCConfig.h \
    Internal/VLCEventManager.h \
    Internal/VLCLibVLCBridging.h \
    Internal/VLCVideoCommon.h

OBJECTIVE_SOURCES += \
    Sources/VLCAudio.m \
    Sources/VLCEventManager.m \
    Sources/VLCLibrary.m \
    Sources/VLCMedia.m \
    Sources/VLCMediaDiscoverer.m \
    Sources/VLCMediaLibrary.m \
    Sources/VLCMediaList.m \
    Sources/VLCMediaListPlayer.m \
    Sources/VLCMediaPlayer.m \
    Sources/VLCMediaThumbnailer.m \
    Sources/VLCServicesDiscoverer.m \
    Sources/VLCStreamOutput.m \
    Sources/VLCStreamSession.m \
    Sources/VLCTime.m \
    Sources/VLCVideoCommon.m \
    Sources/VLCVideoLayer.m \
    Sources/VLCVideoView.m

OTHER_FILES = \
    VLCKit.pri \
    VLCKit_static.pri

# EOF
