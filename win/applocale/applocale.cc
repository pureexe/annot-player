// applocale.cc
// 9/1/2012
//
// See: http://bbs.et8.net/bbs/showthread.php?t=627102
// - SET __COMPAT_LAYER=#ApplicationLocale
// - SET ApplocaleID=0411

#include "win/applocale/applocale.h"
#include "win/qtwin/qtwin.h"
#include <QtCore/QFile>
#include <QtCore/QStringList>

#define ENV_LAYER   "__COMPAT_LAYER"
#define ENV_LCID    "AppLocaleID"

bool
AppLocale::createProcess(const QString &path, int lcid)
{
  //QStringList env;
  //if (lcid) {
  //  env << "__COMPAT_LAYER=#APPLICATIONLOCALE"
  //      << "AppLocaleID=0" + QString::number(lcid, 16);
  //return QtWin::createProcess(path, env);

  QByteArray layer_bak = qgetenv(ENV_LAYER),
             lcid_bak = qgetenv(ENV_LCID);

  qputenv(ENV_LAYER, "#APPLICATIONLOCALE");
  qputenv(ENV_LCID, QString::number(lcid, 16).toLocal8Bit());

  bool ret = QtWin::createProcess(path);

  qputenv(ENV_LAYER, layer_bak);
  qputenv(ENV_LCID, lcid_bak);

  return ret;
}

bool
AppLocale::isAvailable()
{
  return QFile::exists(
    QtWin::getWinDirPath() + "/AppPatch/AlLayer.dll"
  );
}

// EOF
