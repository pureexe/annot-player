// qtunix.cc
// 12/3/2011
//
// Mac devices:
// http://www.filibeto.org/unix/macos/lib/dev/documentation/DeviceDrivers/Conceptual/WorkingWStorage/WorkingWStorage.pdf

#include "qtunix.h"
#include <QtCore>

#define DEVICE_PREFIX "/dev/"

QStringList
QtUnix::getDevicesWithType(DeviceType type)
{
  QStringList ret;
  switch (type) {
  case HardDisk:
#ifdef Q_WS_MAC
    ret = getDevicesWithNameFilter("rdisk*");
#else // UNIX
    {
      QStringList filters("hd*");
      filters.append("sd*");
      ret = getDevicesWithNameFilters(filters);
    }
#endif // Q_WS_MAC
    break;
  case CdRom:
#ifdef Q_WS_MAC
    {
      QStringList disks = getDevicesWithNameFilter("rdisk*");
      if (!disks.isEmpty())
        foreach (QString path, disks)
          if (path.contains(QRegExp("^/dev/rdisk[1-9]$"))) // skip disk0 and partitions
            ret.append(path);
    }
#else
    ret = getDevicesWithNameFilter("cdrom*");
#endif // Q_WS_MAC
    break;
  default: break;
  }
  return ret;
}

QStringList
QtUnix::getDevicesWithNameFilters(const QStringList &filters)
{
  QStringList ret;
  QStringList names = QDir(DEVICE_PREFIX).entryList(filters, QDir::System);

  if (!names.isEmpty())
    foreach (QString fileName, names)
      ret.append(DEVICE_PREFIX + fileName);
  return ret;
}

bool
QtUnix::isDeviceFile(const QString &path)
{
  return QFileInfo(path).exists() &&
         path.startsWith("/dev/");
}

// EOF
