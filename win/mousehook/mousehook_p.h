#pragma once

// mousehook_p.h
// 11/26/2011

#include <qt_windows.h>
#include <QtCore/QObject>

class MouseHookPrivate
{
public:
  HHOOK hook;
  bool windowPosEnabled;
  QObject *listener;

public:
  MouseHookPrivate() : hook(0), windowPosEnabled(false), listener(0) { }
};

// EOF
