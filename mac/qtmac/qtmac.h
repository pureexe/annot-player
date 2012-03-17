#ifndef QTMAC_H
#define QTMAC_H

// qtmac.h
// 11/11/2011

#include <QString>

namespace QtMac {

  // - Environment -

  QString homeLibraryPath();
  QString homeCachesPath();
  QString homeLogsPath();
  QString homeApplicationSupportPath();

  // - Devices -

#ifdef WITH_IOKIT
  QStringList getCDMediaPaths();
#endif // WITH_IOKIT

  // - Shutdown -

  void halt();
  void reboot();
  void sleep();

} // namespace QtMac

#endif // QTMAC_H
