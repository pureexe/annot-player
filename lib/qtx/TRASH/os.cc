// core/os.cc
// 8/7/2011

#include "core/os.h"
#include <QtCore>

// - Directories -

// EOF

/*
QString
Core::Os::
desktopPath()
{
  QString path;

#ifdef Q_OS_WIN
  QSettings reg(
    "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
    QSettings::NativeFormat
  );
  path = reg.value("Desktop").toString();
#else
  path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif // Q_OS_WIN

  if (QDir(path).exists())
    return path;
  else
    return  QDir::homePath();
}
*/
