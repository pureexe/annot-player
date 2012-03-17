#ifndef QTUNIX_H
#define QTUNIX_H

// qtunix.h
// 12/3/2011

#include <QStringList>

namespace QtUnix {

  // - Device -

  enum DeviceType {
    UnknownDevice = 0,
    HardDisk,
    CdRom,
    DeviceTypeCount
  };
  QStringList getDevicesWithType(DeviceType type);

  ///  Return paths, where the name filter something like "sda*".
  QStringList getDevicesWithNameFilters(const QStringList &filters);

  inline QStringList getDevicesWithNameFilter(const QString &filter)
  { return getDevicesWithNameFilters(QStringList(filter)); }

  bool isDeviceFile(const QString &path);

  // - Shutdown -
  void halt();
  void reboot();

} // namespace QtUnix

#endif // QTUNIX_H
