#pragma once

// picker_p.h
// 10/30/2011

#include <qt_windows.h>

class WindowPickerPrivate
{
public:
  HHOOK hook;
  bool singleShot;

public:
  WindowPickerPrivate() : hook(0), singleShot(false) { }
};

// EOF
