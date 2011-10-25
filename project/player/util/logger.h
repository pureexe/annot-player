#ifndef LOGGER_H
#define LOGGER_H

// Logger.h
// 10/16/2011

#include <QString>

namespace Logger {
  void error(const QString &message);
  void log(const QString &message);
  void notify(const QString &message);
  void warn(const QString &message);
} // namespace Logger

#endif // LOGGER_H
