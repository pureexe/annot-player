#pragma once

// ihf_p.h
// 10/15/2011

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtGui/QWidget>

class HookManager;
class TextThread;
class TextThreadDelegate;

class Ihf
{
  Q_DISABLE_COPY(Ihf)

  static HookManager *hookManager_;
  static qint64 messageInterval_;
  static WId parentWindow_;

  static QHash<TextThread *, TextThreadDelegate *> threadDelegates_;
  static QHash<QString, ulong> hookAddresses_;

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

  // - Callbacks -
protected:
  static ulong threadCreateCallback(TextThread *t);
  static ulong threadRemoveCallback(TextThread *t);
  static ulong threadOutputCallback(TextThread *t, uchar *data, ulong dataLength, ulong bNewLine, void *pUserData);
};

// EOF
