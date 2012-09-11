// mecabsettings.cc
// 9/4/2012
// See: mecab/src/utils.cpp

#include "module/mecab/mecabsettings.h"
#include <QtCore/QDir>

//#define DEBUG "mecabsettings"
#include "module/debug/debug.h"

// - Properties -

void
MeCabSettings::setMeCabRcFile(const QString &path)
{ qputenv("MECABRC", QDir::toNativeSeparators(path).toLocal8Bit()); }

// EOF
