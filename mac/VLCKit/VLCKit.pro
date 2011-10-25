# VLCKit framework
# Since 6/30/2011
# TODO: Make this into a framework!

TEMPLATE = lib
#CONFIG += ????
TARGET = vlckit

INCLUDEPATH += Internal
DEPENDPATH +=  Internal

HEADERS += \
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
    Internal/VLCEventManager.h \
    Internal/VLCLibVLCBridging.h \
    Internal/VLCVideoCommon.h

SOURCES += \
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

OTHER_FILES += VLCKit.pri

LIBS        += -lvlc
LIBS        += -framework Foundation -framework AppKit -framework Quartz

INCLUDEPATH += ${HOME}/opt/vlc/include
LIBS        += -L${HOME}/opt/vlc/lib

#DESTDIR = ../../build.mac
DESTDIR = ../../../touch-build-desktop/build.mac

# EOF
