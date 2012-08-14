#pragma once

// texthook.h  10/14/2011

#include "texthook_config.h"
#include <QtCore/QList>
#include <QtGui/QWidget> // where WId is declared

class TextHook : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(TextHook)
  typedef TextHook Self;
  typedef QObject Base;

  // - Constructions -
public:
  TEXTHOOKAPI static Self *globalInstance() { static Self g; return &g; }

signals: // No import/export needed for Qt signals.
  void enabledChanged(bool value);
  //void textReceived(const QString &text, ulong hookId, ulong processId);
  void messageReceived(const QByteArray &data, ulong hookId, ulong processId);
  void processAttached(qint64 pid);
  //void processDetached(qint64 pid);

  // - Properties -
public:
  TEXTHOOKAPI bool isEnabled() const { return enabled_; }
  TEXTHOOKAPI bool isActive() const;

  TEXTHOOKAPI WId parentWinId() const;
  TEXTHOOKAPI void setParentWinId(WId hwnd); ///< Must be set to a valid window so that ::SetTimer works

  TEXTHOOKAPI int interval() const;
  TEXTHOOKAPI void setInterval(int msecs); ///< Interval to differentiate sentence

  // - Profiles -
  // TODO: move game profiles to another class
public:
  TEXTHOOKAPI bool isStandardHookName(const QString &name) const;
  TEXTHOOKAPI bool isKnownHookForProcess(const QString &hookName, const QString &processName) const;

  TEXTHOOKAPI QString hookNameById(ulong hookId) const; ///< Broken in ITH3

  static TEXTHOOKAPI QString guessEncodingForFile(const QString &fileName);

  // - Injection -
public:
  TEXTHOOKAPI bool attachProcess(ulong pid, bool checkActive = false);
  TEXTHOOKAPI bool detachProcess(ulong pid, bool checkActive = false);
  TEXTHOOKAPI void detachAllProcesses();
  TEXTHOOKAPI bool isProcessAttached(ulong pid) const;
  TEXTHOOKAPI bool isEmpty() const { return pids_.isEmpty(); }

public slots:
  TEXTHOOKAPI void setEnabled(bool t);
  TEXTHOOKAPI void start();
  TEXTHOOKAPI void stop();
  TEXTHOOKAPI void clear();

  // - Implementations -
protected:
  explicit TextHook(QObject *parent = nullptr) : Base(parent), enabled_(true) { }
  ~TextHook() { if (isActive()) stop(); }

public:
  //void sendText(const QString &text, ulong tid, ulong pid) // text thread callback
  //{ emit textReceived(text, tid, pid); }

  void sendMessage(const QByteArray &data, ulong tid, ulong pid) // text thread callback
  {
    if (enabled_)
      emit messageReceived(data, tid, pid);
  }

private:
  bool enabled_;
  QList<ulong> pids_;
};

// EOF
