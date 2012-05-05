#ifndef LOGGER_H
#define LOGGER_H

// Logger.h
// 10/16/2011

#include <QtCore/QString>
#include <QtCore/QObject>

namespace Logger {
  void error(const QString &message);
  void log(const QString &message);
  void notify(const QString &message);
  void warn(const QString &message);

  // TODO Lamed. Make Logger as a singleton instead.
  class LoggerSignals : public QObject
  {
    Q_OBJECT
    Q_DISABLE_COPY(LoggerSignals)
    typedef LoggerSignals Self;
    typedef QObject Base;
  public:
    static Self *globalInstance() { static Self g; return &g; }

  protected:
    explicit LoggerSignals(QObject *parent = 0) : Base(parent) { }

  signals:
    void errorMessage(QString msg);
    void message(QString msg);
    void notification(QString msg);
    void warning(QString msg);

  public:
    void showError(const QString &msg) { emit errorMessage(msg); } ///< \internal
    void showMessage(const QString &msg) { emit message(msg); } ///< \internal
    void showNotification(const QString &msg) { emit notification(msg); } ///< \internal
    void showWarning(const QString &msg) { emit warning(msg); } ///< \internal

  public slots:
    void error(const QString &message) { Logger::error(message); }
    void log(const QString &message) { Logger::log(message); }
    void notify(const QString &message) { Logger::notify(message); }
    void warn(const QString &message) { Logger::warn(message); }
  };

} // namespace Logger

#endif // LOGGER_H
