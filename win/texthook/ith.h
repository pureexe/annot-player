#pragma once

// ith.h
// 10/15/2011

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtGui/QWidget>

class HookManager;
class TextThread;
class TextThreadProperty;

class Ith
{
  Q_DISABLE_COPY(Ith)
  typedef Ith Self;
  typedef QObject Base;

  static QHash<TextThread *, TextThreadProperty *> threadProperties_;
  static HookManager *hookManager_;
  static qint64 messageInterval_;
  static WId parentWindow_;

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

  // - Callbacks -
protected:
  static ulong threadCreateCallback(TextThread *t);
  static ulong threadRemoveCallback(TextThread *t);
  static ulong threadOutputCallback(TextThread *t, uchar *data, ulong dataLength, ulong bNewLine, void *pUserData);
};

// EOF
