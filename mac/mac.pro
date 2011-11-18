# mac.pro
# Since 7/30/2011
mac {

TEMPLATE = subdirs
include(qtmac/qtmac.pri)
include(qtstep/qtstep.pri)
include(vlckit_qt/vlckit_qt.pri)
#include(VLCKit/VLCKit.pri)
SUBDIRS = \
  macdeploy \
  macdeployqt \
  VLCKit

}
# EOF
