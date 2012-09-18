// mecabsettings.cc
// 9/4/2012
// See: mecab/src/utils.cpp

#include "lib/mecab/mecabsettings.h"
#include <QtCore/QDir>

//#define DEBUG "mecabsettings"
#include "qtx/qxdebug.h"

// - Properties -

#define ENV_MECABRC "MECABRC"

void
MeCabSettings::setMeCabRcFile(const QString &path)
{ qputenv(ENV_MECABRC, QDir::toNativeSeparators(path).toLocal8Bit()); }

QString
MeCabSettings::getMeCabRcFile()
{ return qgetenv(ENV_MECABRC); }

// EOF
