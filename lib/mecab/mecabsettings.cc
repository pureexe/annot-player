// mecabsettings.cc
// 9/4/2012
// See: mecab/src/utils.cpp

#include "lib/mecab/mecabsettings.h"
#include <QtCore/QDir>

//#define DEBUG "mecabsettings"
#include "lib/debug/debug.h"

// - Properties -

void
MeCabSettings::setMeCabRcFile(const QString &path)
{ qputenv("MECABRC", QDir::toNativeSeparators(path).toLocal8Bit()); }

// EOF
