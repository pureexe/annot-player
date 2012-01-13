#pragma once

// 1/10/2011
// ith/query.h

#include <qt_windows.h>

bool GetProcessPath(HANDLE hProc, LPWSTR path);
bool GetProcessPath(DWORD pid, LPWSTR path);

// EOF
