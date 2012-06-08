#pragma once

// 1/10/2011
// ith/inject.h

#include <qt_windows.h>

HANDLE InjectProcess(HANDLE hProc);
HANDLE InjectProcessByPid(DWORD pid);

BOOL DetachProcessByPid(DWORD pid);

// EOF
