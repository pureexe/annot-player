// qtunix.cc
// 12/3/2011
//
// Mac devices:
// http://www.filibeto.org/unix/macos/lib/dev/documentation/DeviceDrivers/Conceptual/WorkingWStorage/WorkingWStorage.pdf

#include "qtunix/qtunix.h"
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QStringList>

#ifdef Q_OS_MAC
# include <paths.h>
# define DEVICE_PREFIX _PATH_DEV
#else
# define DEVICE_PREFIX "/dev/"
#endif // Q_OS_MAC

#define DEBUG "qtunix"
#include "lib/debug/debug.h"

QStringList
QtUnix::getDevicesWithType(DeviceType type)
{
  QStringList ret;
  switch (type) {
  case HardDisk:
#ifdef Q_OS_MAC
    ret = getDevicesWithNameFilter("rdisk*");
#else // UNIX
    {
      QStringList filters("hd*");
      filters.append("sd*");
      ret = getDevicesWithNameFilters(filters);
    }
#endif // Q_OS_MAC
    break;
  case CdRom:
#ifdef Q_OS_MAC
    {
      QStringList disks = getDevicesWithNameFilter("rdisk*");
      if (!disks.isEmpty())
        foreach (const QString &path, disks)
          if (path.contains(QRegExp("^/dev/rdisk[1-9]$"))) // skip disk0 and partitions
            ret.append(path);
    }
#else
    ret = getDevicesWithNameFilter("cdrom*");
#endif // Q_OS_MAC
    break;
  default: ;
  }
  return ret;
}

QStringList
QtUnix::getDevicesWithNameFilters(const QStringList &filters)
{
  QStringList ret;
  QStringList names = QDir(DEVICE_PREFIX).entryList(filters, QDir::System);

  if (!names.isEmpty())
    foreach (const QString &fileName, names)
      ret.append(DEVICE_PREFIX + fileName);
  return ret;
}

bool
QtUnix::isDeviceFile(const QString &path)
{
  return QFileInfo(path).exists() &&
         path.startsWith("/dev/");
}

// - Commands -

int
QtUnix::run(const QString &bin, const QStringList &args, int timeout)
{
  DOUT("enter: bin =" << bin << ", args =" << args << ", timeout =" << timeout);
  QProcess proc;
  proc.start(bin, args, QIODevice::ReadOnly | QIODevice::Text);
  proc.waitForFinished(timeout);
  int err = proc.exitCode();
  DOUT("exit: err =" << err << ", out =" << proc.readAll());
  return err;
}

bool
QtUnix::cp(const QStringList &from, const QString &to, const QString &opt)
{
  if (from.isEmpty() || to.isEmpty())
    return false;
  QStringList args;
  if (!opt.isEmpty())
    args.append(opt);
  args.append(from);
  args.append(to);
  return !run("cp", args);
}

bool
QtUnix::mv(const QStringList &from, const QString &to, const QString &opt)
{
  if (from.isEmpty() || to.isEmpty())
    return false;
  QStringList args;
  if (!opt.isEmpty())
    args.append(opt);
  args.append(from);
  args.append(to);
  return !run("mv", args);
}

bool
QtUnix::halt()
{ return QProcess::startDetached("shutdown -h now"); }

bool
QtUnix::reboot()
{ return QProcess::startDetached("shutdown -r now"); }

// EOF
