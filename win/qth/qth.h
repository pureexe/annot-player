#pragma once

// qth.h  10/14/2011

#include "qth_config.h"
#include <QObject>
#include <QWidget> // where WId is declared
#include <QList>

//class QthImpl;
class Qth : public QObject
{
  Q_OBJECT
  typedef Qth Self;
  typedef QObject Base;
  //typedef QthImpl Impl;

  // - Constructions -
public:
  QTHAPI static Self *globalInstance();

signals: // No import/export needed for Qt signals.
  void textReceived(const QString &text, ulong hookId, ulong processId, const QString &hookName);

  // - Properties -
public:
  QTHAPI WId parentWinId() const;
  QTHAPI void setParentWinId(WId hwnd); ///< Must be set to a valid window so that ::SetTimer works

  QTHAPI int interval() const;
  QTHAPI void setInterval(int msecs); ///< Interval to differentiate sentence

  // - Queries -
public:
  QTHAPI bool isElevated() const;

  QTHAPI bool isStandardHookName(const QString &name) const;
  QTHAPI bool isKnownHookForProcess(const QString &hookName, const QString &processName) const;

  QTHAPI QString hookNameById(ulong hookId) const;

  QTHAPI ulong processIdByName(const QString &name) const;

  // - Injection -
public:
  QTHAPI bool attachProcess(ulong pid, bool checkActive = false);
  QTHAPI bool detachProcess(ulong pid, bool checkActive = false);
  QTHAPI void detachAllProcesses();
  QTHAPI bool isProcessAttached(ulong pid) const;
  QTHAPI bool isEmpty() const;

public slots:
  QTHAPI void clear();

  // - Implementations -
protected:
  explicit Qth(QObject *parent = 0);
  ~Qth();
public:
  void sendText(const QString &text, ulong threadId, ulong pid, const QString &hookName) // Ith callback
  { emit textReceived(text, threadId, pid, hookName); }

private:
  QList<ulong> pids_;
};

// EOF
