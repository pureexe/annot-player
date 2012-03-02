# mac.pro
# 7/30/2011
mac {

TEMPLATE = subdirs
include(qtmac/qtmac.pri)
include(qtstep/qtstep.pri)
include(vlcstep/vlcstep.pri)
#include(VLCKit/VLCKit.pri)
#SUBDIRS += macdeploy
#SUBDIRS += macdeployqt
SUBDIRS = VLCKit

}
# EOF
