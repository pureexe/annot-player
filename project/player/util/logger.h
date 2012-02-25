#ifndef LOGGER_H
#define LOGGER_H

// Logger.h
// 10/16/2011

#include <QString>
#include <QObject>

namespace Logger {
  void error(const QString &message);
  void log(const QString &message);
  void notify(const QString &message);
  void warn(const QString &message);

  // TODO Lamed. Make Logger as a singleton instead.
  class LoggerSignals : public QObject
  {
    Q_OBJECT
    typedef LoggerSignals Self;
    typedef QObject Base;
  public:
    static Self *globalInstance() { static Self g; return &g; }

  protected:
    explicit LoggerSignals(QObject *parent = 0) : Base(parent) { }

  signals:
    void errored(QString msg);
    void logged(QString msg);
    void notified(QString msg);
    void warned(QString msg);

  public:
    void emit_errored(const QString &msg) { emit errored(msg); } ///< \internal
    void emit_logged(const QString &msg) { emit logged(msg); } ///< \internal
    void emit_notified(const QString &msg) { emit notified(msg); } ///< \internal
    void emit_warned(const QString &msg) { emit warned(msg); } ///< \internal

  public slots:
    void error(const QString &message) { Logger::error(message); }
    void log(const QString &message) { Logger::log(message); }
    void notify(const QString &message) { Logger::notify(message); }
    void warn(const QString &message) { Logger::warn(message); }
  };

} // namespace Logger

#endif // LOGGER_H
