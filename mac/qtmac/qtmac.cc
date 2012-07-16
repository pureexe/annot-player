// qtmac.cc
// 11/11/2011

#include "qtmac/qtmac.h"
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QStringList>
#ifdef WITH_IOKIT
# include <IOKit/IOBSD.h>
# include <IOKit/IOKitLib.h>
# include <IOKit/storage/IOCDMedia.h>
# include <IOKit/storage/IOCDTypes.h>
# include <IOKit/storage/IOMedia.h>
# include <CoreFoundation/CoreFoundation.h>
#endif // WITH_IOKIT
#include <cstring>
#include <sys/param.h>
#include <sys/xattr.h>
#include <paths.h>

#define DEBUG "qtmac"
#include "module/debug/debug.h"

// - Environment -

QString
QtMac::homeLibraryPath()
{ return QDir::homePath() + "/Library"; }

QString
QtMac::homeMusicPath()
{ return QDir::homePath() + "/Music"; }

QString
QtMac::homeMoviesPath()
{ return QDir::homePath() + "/Movies"; }

QString
QtMac::homeDownloadsPath()
{ return QDir::homePath() + "/Downloads"; }

QString
QtMac::homeDocumentsPath()
{ return QDir::homePath() + "/Documents"; }

QString
QtMac::homeCachesPath()
{ return homeLibraryPath() + "/Caches"; }

QString
QtMac::homeLogsPath()
{ return homeLibraryPath() + "/Logs"; }

QString
QtMac::homeApplicationSupportPath()
{ return homeLibraryPath() + "/Application Support"; }

// - Devices -

#ifdef WITH_IOKIT

// See: WorkingWStorage.pdf
namespace { namespace detail {
  kern_return_t getCDMediaIterator(io_iterator_t *mediaIterator)
  {
    mach_port_t masterPort;

    kern_return_t ret = ::IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (!ret != KERN_SUCCESS)
      return ret;

    CFMutableDictionaryRef matching = ::IOServiceMatching(kIOCDMediaClass);
    if (!matching)
      ret = KERN_FAILURE;
    else {
      ::CFDictionarySetValue(matching, CFSTR(kIOMediaEjectableKey), kCFBooleanTrue);
      ret = ::IOServiceGetMatchingServices(masterPort, matching, mediaIterator);
    }
    return ret;
  }

} } // anonymous detail

QStringList
QtMac::getCDMediaPaths()
{
  QStringList ret;
  io_iterator_t mediaIterator;
  if (detail::getCDMediaIterator(&mediaIterator) == KERN_SUCCESS && mediaIterator) {
    char buf[MAXPATHLEN];
    io_iterator_t current = ::IOIteratorNext(mediaIterator);
    while (current) {
      CFStringRef cfstr = static_cast<CFStringRef>(
        ::IORegistryEntryCreateCFProperty(
           current, CFSTR(kIOBSDNameKey), kCFAllocatorDefault, (IOOptionBits)0
      ));
      if (cfstr) {
        qstrcpy(buf, _PATH_DEV);
        ::strcat(buf, "r"); // r-disk
        size_t len = qstrlen(buf);
        if (::CFStringGetCString(cfstr, buf + len, sizeof(buf) - len, kCFStringEncodingASCII))
          ret.append(buf);
        CFRelease(cfstr);
      }

      io_iterator_t previous = current;
      current = ::IOIteratorNext(current);
      ::IOObjectRelease(previous);
    }
  }
  return ret;
}

#endif // WITH_IOKIT

// - Commands -

int
QtMac::run(const QString &bin, const QStringList &args, int timeout)
{
  //DOUT("enter: bin =" << bin << ", args =" << args << ", timeout =" << timeout);
  QProcess proc;
  proc.start(bin, args, QIODevice::ReadOnly | QIODevice::Text);
  proc.waitForFinished(timeout);
  int err = proc.exitCode();
  //DOUT("exit: err =" << err << ", out =" << proc.readAll());
  return err;
}

bool
QtMac::open(const QString &app, const QStringList &args)
{
  DOUT("app =" << app << ", args =" << args);
  return QProcess::startDetached("open", QStringList()
    <<"-a"
    << app
    << args
  );
}

bool
QtMac::halt()
{
  DOUT("enter");
  //QProcess::startDetached("shutdown -s now");
  bool ret = QProcess::startDetached("osascript -e '"
    "tell app \"Finder\" to shut down"
  "'");
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
QtMac::reboot()
{
  DOUT("enter");
  //QProcess::startDetached("shutdown -r now");
  bool ret = QProcess::startDetached("osascript", QStringList() << "-e" <<
    "tell app \"Finder\" to shut down"
  );
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
QtMac::sleep()
{
  DOUT("enter");
  //QProcess::startDetached("pmset sleepnow");
  bool ret = QProcess::startDetached("osascript", QStringList() << "-e" <<
    "tell app \"Finder\" to sleep"
  );
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Attributes -

// As SetFile only comes with Xcode, use xattr instead
bool
QtMac::setFileAttributes(const QString &fileName, uint flags)
{
  DOUT("enter: fileName =" << fileName << ", flags =" << flags);
  QByteArray value = !flags ? XA_FINDERINFO_NULL :
                      flags & FA_CustomIcon ? XA_FINDERINFO_CUSTOMICON :
                      "";
  bool ret = !value.isEmpty() && setFileExtendedAttribute(fileName, XA_FINDERINFO, value, TextMode);
  DOUT("exit: ret =" << ret);
  return ret;
}

bool QtMac::removeFileAttributes(const QString &fileName)
{
  return !run("xattr", QStringList()
    << "-d"
    << XA_FINDERINFO
    << fileName
  );
}

bool
QtMac::setFileExtendedAttribute(const QString &fileName, const char *key, const QByteArray &value, Mode mode)
{
  // See: man setxattr
  // See: http://www.cocoanetics.com/2012/03/reading-and-writing-extended-file-attributes/
  switch (mode) {
  case BinaryMode:
    return !::setxattr(fileName.toUtf8(), key, value, value.size(), 0, 0);
  case TextMode:
    return !run("xattr", QStringList() << "-wx"
      << QString::fromLocal8Bit(key)
      << value
      << fileName
    );
  }
  Q_ASSERT(0);
  return false;
}


// EOF
