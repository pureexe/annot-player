#pragma once

// mousehook.h
// 11/26/2011

#include "mousehook_config.h"

QT_FORWARD_DECLARE_CLASS(QObject)

class MouseHookImpl; ///< \internal

///  This class should be used as a singleton.
class MouseHook
{
  //Q_OBJECT
  typedef MouseHook Self;
  //typedef QObject Base;
  typedef MouseHookImpl Impl;

  // - Singleton -
public:
  MOUSEHOOKAPI static Self *globalInstance();

  // - Properties -
public:
  MOUSEHOOKAPI bool isActive() const;

  MOUSEHOOKAPI bool isWindowPosEnabled() const; ///< If capture relative window pos, default false

  //MOUSEHOOKAPI const QList<QObject*> &eventListeners() const;
  //MOUSEHOOKAPI void addEventListener(QObject *listener);
  //MOUSEHOOKAPI void removeEventListener(QObject *listener);
  //MOUSEHOOKAPI void clearEventListeners();

  MOUSEHOOKAPI QObject *eventListener() const;
  MOUSEHOOKAPI void setEventListener(QObject *listener);

  // - Slots -
public:
  MOUSEHOOKAPI void start();       ///< Start getting the winId that the user clicked
  MOUSEHOOKAPI void stop();        ///< Stop hook service

  MOUSEHOOKAPI void setWindowPosEnabled(bool enabled);

  // - Implementation -
protected:
  //explicit MouseHook(QObject *parent = 0);   ///< \internal
  MouseHook();   ///< \internal
  ~MouseHook();  ///< \internal

public:
  void *hook() const; ///< \internal Return global hook handle

private:
  Impl *impl_;
};

// EOF
