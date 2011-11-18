// qtmac.cc
// 11/11/2011

#include "qtmac.h"
#include <QtCore>

#ifndef Q_WS_MAC
  #error "This file is for mac only."
#endif // Q_WS_MAC

// - Environment -

QString
QtMac::homeLibraryPath()
{ return QDir::homePath() + "/Library"; }

QString
QtMac::homeCachesPath()
{ return homeLibraryPath() + "/Caches"; }

QString
QtMac::homeLogsPath()
{ return homeLibraryPath() + "/Logs"; }


// EOF
