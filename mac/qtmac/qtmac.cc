// qtmac.cc
// 11/11/2011

#include <QtGlobal>
#ifndef Q_WS_MAC
  #error "This file is for mac only."
#endif // Q_WS_MAC

#include "qtmac.h"
#include <QtCore>

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

QString
QtMac::homeApplicationSupportPath()
{ return homeLibraryPath() + "/Application Support"; }


// EOF
