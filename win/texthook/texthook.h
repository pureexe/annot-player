#pragma once

// texthook.h  10/14/2011

#include "texthook_config.h"
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtGui/QWidget> // where WId is declared

class TextHook : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(TextHook)
  typedef TextHook Self;
  typedef QObject Base;

  // - Construction -
public:
  TEXTHOOKAPI static Self *globalInstance() { static Self g; return &g; }

  TEXTHOOKAPI static int capacity() { return 0x20; } // see: ITH/common.h

signals: // No import/export needed for Qt signals.
  void enabledChanged(bool value);
  void messageReceived(const QByteArray &data, qint64 signature, const QString &provider);

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

  TEXTHOOKAPI static const char *addedHookName() { return "Cloud"; }

  // - Profiles -
  // TODO: move game profiles to another class
public:
  //TEXTHOOKAPI bool isStandardHookName(const QString &name) const;
  //TEXTHOOKAPI bool isKnownHookForProcess(const QString &hookName, const QString &processName) const;

  //TEXTHOOKAPI QString hookNameById(ulong hookId) const; ///< Broken in ITH3

  static TEXTHOOKAPI QString guessEncodingForFile(const QString &fileName);

  // - Injection -
public:
  TEXTHOOKAPI bool attachOneProcess(ulong pid, bool checkActive = false);
  TEXTHOOKAPI bool attachProcess(ulong pid, bool checkActive = false);
  TEXTHOOKAPI bool detachProcess(ulong pid, bool checkActive = false);
  TEXTHOOKAPI void detachAllProcesses();
  TEXTHOOKAPI QList<ulong> attachedProcesses(bool checkActive = false) const;
  TEXTHOOKAPI ulong anyAttachedProcess(bool checkActive = false) const;
  TEXTHOOKAPI ulong currentProccess() const { return anyAttachedProcess(true); } // check active = true

  TEXTHOOKAPI bool containsProcess(ulong pid) const { return pids_.contains(pid); }
  TEXTHOOKAPI bool isEmpty() const { return pids_.isEmpty(); }

  TEXTHOOKAPI bool addHook(ulong pid, const QString &code);
  TEXTHOOKAPI bool containsHook(ulong pid) const { return hooks_.contains(pid); }
  TEXTHOOKAPI bool containsHook(ulong pid, const QString &code) const
  { return processHook(pid) == code; }
  TEXTHOOKAPI QString processHook(ulong pid) const;
  TEXTHOOKAPI QString currentHook() const { return processHook(currentProccess()); }
  TEXTHOOKAPI bool removeHook(ulong pid);

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

  void sendMessage(const QByteArray &text, qint64 signature, const QString &provider) // text thread callback
  { if (enabled_) emit messageReceived(text, signature, provider); }

private:
  bool enabled_;
  QList<ulong> pids_;
  QHash<ulong, QString> hooks_; // ITH hook code, indexed by pid
};

// EOF
