#ifndef QTMAC_H
#define QTMAC_H

// qtmac.h
// 11/11/2011

#include <QString>
#include <QStringList>

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

  // - Applescript -

  bool open(const QString &app, const QStringList &args = QStringList());

  bool halt();
  bool reboot();
  bool sleep();

} // namespace QtMac

#endif // QTMAC_H
