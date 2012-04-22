#ifndef QTUNIX_H
#define QTUNIX_H

// qtunix.h
// 12/3/2011

#include <QtCore/QStringList>

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

  // - Commands -

  int run(const QString &bin, const QStringList &args = QStringList(), int timeoutMsecs = -1);

  bool cp(const QStringList &from, const QString &to, const QString &opt = QString());
  inline bool cp(const QString &from, const QString &to, const QString &opt = QString())
  { return cp(QStringList(from), to, opt); }

  bool mv(const QStringList &from, const QString &to, const QString &opt = QString());
  inline bool mv(const QString &from, const QString &to, const QString &opt = QString())
  { return mv(QStringList(from), to, opt); }

  bool halt();
  bool reboot();

} // namespace QtUnix

#endif // QTUNIX_H
