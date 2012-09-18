#pragma once

// mousehook_p.h
// 11/26/2011

#include <qt_windows.h>

class QObject;

class MouseHookPrivate
{
public:
  HHOOK hook;
  bool windowPosEnabled;
  QObject *listener;

public:
  MouseHookPrivate() : hook(nullptr), windowPosEnabled(false), listener(nullptr) { }
};

// EOF
