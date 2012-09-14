// qtnt.cc
// 10/14/2011

#include "qtwinnt/ntdll.h"
#include "qtwinnt/qtwinnt.h"
#include <QtCore>
#include <qt_windows.h>
#include <string>

#define DEBUG "qtwinnt"
#include "lib/debug/debug.h"

// - Process Queries -

bool
QtWinnt::isElevated()
{
  DOUT("enter");
  static int ret = -1;
  if (ret < 0)
    ret = isProcessElevated(NtCurrentProcess()) ? 1 : 0;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
QtWinnt::isProcessElevated(HANDLE hProc)
{
  HANDLE hToken;
  DWORD dwRet;
  TOKEN_PRIVILEGES priv = { 1, {0x14, 0} }; {
    priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  }

  ::NtOpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
  bool ret = !::NtAdjustPrivilegesToken(hToken, FALSE, &priv, sizeof(priv), NULL, &dwRet);
  ::NtClose(hToken);
  return ret;
}


DWORD
QtWinnt::getProcessIdByName(const QString &name)
{
  std::wstring wsName = name.toStdWString();
  LPWSTR pwcTarget = const_cast<LPWSTR>(wsName.c_str()); // the same life time as wsName

  DWORD dwSize = 0x20000;

  BYTE *pbBuffer;
  DWORD dwPid = 0;
  DWORD dwStatus;

  while (true) { // busy querying ... orz
    pbBuffer = new BYTE[dwSize];
    dwStatus = ::NtQuerySystemInformation(SystemProcessInformation, pbBuffer, dwSize, 0);
    if (dwStatus == 0)
      break;
    delete pbBuffer;
    if (dwStatus != STATUS_INFO_LENGTH_MISMATCH)
      return 0;
    dwSize <<= 1;
  }

  auto spiProcessInfo = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(pbBuffer);
  while (spiProcessInfo->dNext) {
    spiProcessInfo = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>
      ((DWORD)spiProcessInfo + spiProcessInfo->dNext);
    if (!::_wcsicmp(pwcTarget, spiProcessInfo->usName.Buffer)) {
      dwPid = spiProcessInfo->dUniqueProcessId;
      break;
    }
  }
  //if (!dwPid) {
  //  ConsoleOutput(ErrorNoProcess);
  //  DOUT("process not found");
  //}
  delete pbBuffer;
  return dwPid;
}


// EOF
