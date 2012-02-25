# VLCKit.pri
# 7/30/2011
# TODO: Make this into a framework!

INCLUDEPATH += $$PWD/..

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD
#INCLUDEPATH += $$PWD/Internal
#DEPENDPATH  += $$PWD/Internal
#INCLUDEPATH += $$PWD/Config
#DEPENDPATH  += $$PWD/Config

#LIBS += -framework VLCKit
LIBS += -lvlckit

HEADERS += \
    $$PWD/VLCAudio.h \
    $$PWD/VLCKit.h \
    $$PWD/VLCLibrary.h \
    $$PWD/VLCMedia.h \
    $$PWD/VLCMediaDiscoverer.h \
    $$PWD/VLCMediaLibrary.h \
    $$PWD/VLCMediaList.h \
    $$PWD/VLCMediaListPlayer.h \
    $$PWD/VLCMediaPlayer.h \
    $$PWD/VLCMediaThumbnailer.h \
    $$PWD/VLCServicesDiscoverer.h \
    $$PWD/VLCStreamOutput.h \
    $$PWD/VLCStreamSession.h \
    $$PWD/VLCTime.h \
    $$PWD/VLCVideoLayer.h \
    $$PWD/VLCVideoView.h
#   $$PWD/Config/VLCConfig.h \
#   $$PWD/Internal/VLCEventManager.h \
#   $$PWD/Internal/VLCLibVLCBridging.h \
#   $$PWD/Internal/VLCVideoCommon.h

#OBJECTIVE_SOURCES += \
#    $$PWD/Sources/VLCAudio.m \
#    $$PWD/Sources/VLCEventManager.m \
#    $$PWD/Sources/VLCLibrary.m \
#    $$PWD/Sources/VLCMedia.m \
#    $$PWD/Sources/VLCMediaDiscoverer.m \
#    $$PWD/Sources/VLCMediaLibrary.m \
#    $$PWD/Sources/VLCMediaList.m \
#    $$PWD/Sources/VLCMediaListPlayer.m \
#    $$PWD/Sources/VLCMediaPlayer.m \
#    $$PWD/Sources/VLCMediaThumbnailer.m \
#    $$PWD/Sources/VLCServicesDiscoverer.m \
#    $$PWD/Sources/VLCStreamOutput.m \
#    $$PWD/Sources/VLCStreamSession.m \
#    $$PWD/Sources/VLCTime.m \
#    $$PWD/Sources/VLCVideoCommon.m \
#    $$PWD/Sources/VLCVideoLayer.m \
#    $$PWD/Sources/VLCVideoView.m
#
#LIBS    += \
#    -framework Foundation \
#    -framework AppKit \
#    -framework Quartz

# EOF
