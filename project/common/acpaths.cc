// ac/paths.cc
// 4/23/2012

#include "project/common/acpaths.h"
#ifdef Q_WS_MAC
#  include "mac/qtmac/qtmac.h"
#  include "unix/qtunix/qtunix.h"
#endif // Q_WS_MAC
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#include <QtGui/QDesktopServices>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>

#define DEBUG "acpath"
#include "module/debug/debug.h"

#ifdef Q_WS_MAC
#  define REZ_ICON_VIDEO  QCoreApplication::applicationDirPath() + "/../Resources/" "video.IconRez"
#endif // Q_WS_MAC

QString
AcLocationManager::downloadsLocation() const
{
  QString desktop = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString ret =  desktop + "/" + tr("Video");
  return ret;
}

void
AcLocationManager::createDownloadsLocation()
{
  QString desktop = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString path =  desktop + "/" + tr("Video");

#ifdef Q_WS_MAC
  QString targetIcon = path + "/" "Icon\r";
  if (QFile::exists(targetIcon))
    return;

  QDir dir(path);
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());
  if (!dir.exists()) {
    DOUT("warning: failed to create downloads folder on desktop");
    return;
  }

  QString srcIcon = REZ_ICON_VIDEO;
  bool ok = QFile::exists(srcIcon) &&
    QtUnix::cp(srcIcon, targetIcon) &&
    QtMac::setFileAttributes(path, QtMac::FA_CustomIcon);
  Q_UNUSED(ok);
  DOUT("ok =" << ok);

#elif defined Q_WS_WIN
  QString targetIni = path + "/" "desktop.ini";
  if (QFile::exists(targetIni))
    return;

  QDir dir(path);
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());
  if (!dir.exists()) {
    DOUT("warning: failed to create downloads folder on desktop");
    return;
  }

  QString srcIni = QtWin::getVideoPath() + "/" "desktop.ini";
  bool ok = QFile::exists(srcIni) &&
    QFile::copy(srcIni, targetIni) &&
    QtWin::setFileAttributes(targetIni, QtWin::SystemAttribute | QtWin::HiddenAttribute | QtWin::ArchiveAttribute) &&
    QtWin::setFileAttributes(path, QtWin::ReadOnlyAttribute);
  Q_UNUSED(ok);
  DOUT("ok =" << ok);

#else
  QDir dir(path);
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());
  if (!dir.exists())
    DOUT("warning: failed to create downloads folder on desktop");
#endif // Q_OS_
}

// EOF
