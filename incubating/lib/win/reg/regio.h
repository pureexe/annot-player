#pragma once

// regio.h
// 9/8/2012 jichi

#include <QtCore/QSettings>

namespace RegIO {

  inline QVariant
  readValue(const QString &path, const QString &key, const QVariant &defval = QVariant())
  { return QSettings(path, QSettings::NativeFormat).value(key, defval); }

} // namespace RegIO

// EOF
