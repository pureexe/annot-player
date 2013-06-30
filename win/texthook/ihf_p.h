#pragma once

// ihf_p.h
// 10/15/2011

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtGui/QWidget>
#include <set>

class HookManager; // opaque
class TextThread; // opaque
class TextThreadDelegate;

class Ihf
{
  Q_DISABLE_COPY(Ihf)

  static HookManager *hookManager_;
  static qint64 messageInterval_;
  static WId parentWindow_;
  static bool blacklistEnabled_;

  static QHash<TextThread *, TextThreadDelegate *> threadDelegates_;
  static QHash<QString, ulong> hookAddresses_;
  static std::set<qint64> blacklist_; // Thread signature blacklist

public:

  // - Initialization -
  static bool load();
  static bool isLoaded() { return hookManager_; }
  static void unload();

  // - Properties -
  static WId parentWindow() { return parentWindow_; }
  static void setParentWindow(WId hwnd) { parentWindow_ = hwnd; }

  static qint64 messageInterval() { return messageInterval_; }
  static void setMessageInterval(qint64 msecs) { messageInterval_ = msecs; }

  // - Injection -
  static bool attachProcess(ulong pid);
  static bool detachProcess(ulong pid);

  static bool addHook(ulong pid, const QString &code, const QString &name = QString());
  static bool updateHook(ulong pid, const QString &code);
  static bool removeHook(ulong pid, const QString &code);

  // - Filter -

  static bool blacklistEnabled() { return blacklistEnabled_; }
  static void setBlacklistEnabled(bool t) { blacklistEnabled_ = t; }

  static std::set<qint64> &blacklist() { return blacklist_; }

  // - Callbacks -
protected:
  //static ulong processAttach(ulong pid);
  //static ulong processDetach(ulong pid);
  //static ulong processNewHook(ulong pid);

  static ulong threadCreate(TextThread *t);
  static ulong threadRemove(TextThread *t);
  static ulong threadOutput(TextThread *t, uchar *data, ulong dataLength, ulong bNewLine, void *pUserData);
  //static ulong threadReset(TextThread *t);
  //static ulong threadFilter(TextThread *t);
};

// EOF
