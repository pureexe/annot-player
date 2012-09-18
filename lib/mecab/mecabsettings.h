#ifndef MECABSETTINGS_H
#define MECABSETTINGS_H

// 9/4/2012
// mecabsettings.h

#include <QtCore/QString>

namespace MeCabSettings
{
  void setMeCabRcFile(const QString &path);
  QString getMeCabRcFile() ;
} // namespace MeCabSettings

#endif // MECABSETTINGS_H
