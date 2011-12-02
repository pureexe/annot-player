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
  QTHAPI static Self *globalInstance(); ///< default return null;

protected:
  explicit Qth(QObject *parent = 0);
  ~Qth();

signals:
  void textReceived(const QString &text, int hookId, qint64 tsMsecs);

  // - Properties -
public:
  QTHAPI void setParentWinId(WId hwnd); ///< Must be set to a valid window so that ::SetTimer works

  QTHAPI bool isElevated() const;

  // - Injections -
public:
  QTHAPI bool attachProcess(ulong pid, bool checkActive = false);
  QTHAPI bool detachProcess(ulong pid, bool checkActive = false);
  QTHAPI bool isProcessAttached(ulong pid) const;
  QTHAPI bool isEmpty() const;

public slots:
  QTHAPI void clear();

  // - Debugging -
public:
  QTHAPI ulong getProcessIdByName(const QString &name) const;

  // - Implementations -
public:
  // FIXME: bad design pattern. It is better to pass the callback function to each TextThread.
  QTHAPI void emit_textReceived(const QString &text, int hookId, qint64 tsMsecs); ///< Ith callback

private:
  QList<ulong> pids_;
};

// EOF
