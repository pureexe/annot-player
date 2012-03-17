// qtmac.cc
// 11/11/2011

#include "qtmac.h"
#include <QtCore>
#ifdef WITH_IOKIT
#  include <IOKit/IOKitLib.h>
#  include <IOKit/IOBSD.h>
#  include <IOKit/storage/IOMedia.h>
#  include <IOKit/storage/IOCDMedia.h>
#  include <IOKit/storage/IOCDTypes.h>
#  include <CoreFoundation/CoreFoundation.h>
#endif // WITH_IOKIT
#include <sys/param.h>
#include <paths.h>

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

// - Devices -

#ifdef WITH_IOKIT

// See: WorkingWStorage.pdf
namespace { // anonymous
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

} // anonymous namespace

QStringList
QtMac::getCDMediaPaths()
{
  QStringList ret;
  io_iterator_t mediaIterator;
  if (::getCDMediaIterator(&mediaIterator) == KERN_SUCCESS && mediaIterator) {
    char buf[MAXPATHLEN];
    io_iterator_t current = ::IOIteratorNext(mediaIterator);
    while (current) {
      CFStringRef cfstr = static_cast<CFStringRef>(
        ::IORegistryEntryCreateCFProperty(
           current, CFSTR(kIOBSDNameKey), kCFAllocatorDefault, (IOOptionBits)0
      ));
      if (cfstr) {
        ::strcpy(buf, _PATH_DEV);
        ::strcat(buf, "r"); // r-disk
        size_t len = ::strlen(buf);
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

// - Shutdown -

void
QtMac::halt()
{
  //QProcess::startDetached("shutdown -s now");
  QProcess::startDetached("osascript -e '"
    "tell app \"Finder\" to shut down"
  "'");
}

void
QtMac::reboot()
{
  //QProcess::startDetached("shutdown -r now");
  QProcess::startDetached("osascript", QStringList() << "-e" <<
    "tell app \"Finder\" to shut down"
  );
}

void
QtMac::sleep()
{
  //QProcess::startDetached("pmset sleepnow");
  QProcess::startDetached("osascript", QStringList() << "-e" <<
    "tell app \"Finder\" to sleep"
  );
}

// EOF
