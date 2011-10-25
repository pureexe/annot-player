// main.cc
// 9/3/2011

#include "config.h"
#include <string>
#include <memory>

#ifdef _MSC_VER
  #include <windows.h>
#endif // _MSC_VER

//#define DEBUG "main"
//#include "module/debug/debug.h"
#ifdef DEBUG
  #include <QtCore>
  #define DOUT(_msg)    qDebug() << "main:" << _msg
#else
  #define DOUT(_dummy)
#endif // DEBUG

// - Launcher -

namespace { // anonymous

  inline std::string dirname(const std::string &path)
  { return path.substr(0, path.find_last_of('\\')); }

  inline std::wstring dirname(const std::wstring &path)
  { return path.substr(0, path.find_last_of(L'\\')); }

} // anonymous namespace


// - Main -

int
main(int argc, char *argv[])
{
#ifdef _MSC_VER
  enum { BUF_SIZE = MAX_PATH };
  WCHAR buf[BUF_SIZE]; {
    int nSize = ::GetModuleFileNameW(0, buf, BUF_SIZE);
    if (nSize == BUF_SIZE)
      return -1;
  }
  std::wstring wszApp(buf);
  std::wstring wszNextApp = dirname(wszApp) + (L"\\" APP_PREFIX APP_EXE);
  std::wstring wszNextAppPath = dirname(wszNextApp);

  DOUT(QString::fromStdWString(app));

  STARTUPINFOW siStartupInfo;
  ::memset(&siStartupInfo, 0, sizeof(siStartupInfo));
  siStartupInfo.cb = sizeof(siStartupInfo);

  PROCESS_INFORMATION piProcessInfo;
  ::memset(&piProcessInfo, 0, sizeof(piProcessInfo));

  LPWSTR pwszParam = 0; // TODO
  LPVOID lpEnvironment = 0;     // TODO
  // See: http://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx
  //
  // BOOL WINAPI CreateProcess(
  //   __in_opt     LPCTSTR lpApplicationName,
  //   __inout_opt  LPTSTR lpCommandLine,
  //   __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
  //   __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
  //   __in         BOOL bInheritHandles,
  //   __in         DWORD dwCreationFlags,
  //   __in_opt     LPVOID lpEnvironment,
  //   __in_opt     LPCTSTR lpCurrentDirectory,
  //   __in         LPSTARTUPINFO lpStartupInfo,
  //   __out        LPPROCESS_INFORMATION lpProcessInformation
  // );
  //
  BOOL bResult = ::CreateProcessW(
    wszNextApp.c_str(), // app path
    pwszParam,          // app param
    0, 0,               // security attributes
    false,              // inherited
    CREATE_DEFAULT_ERROR_MODE, // creation flags
    lpEnvironment,
    wszNextAppPath.c_str(),
    &siStartupInfo,
    &piProcessInfo
  );

  return bResult ? 0 : -1;
#else
  // TODO
  return 0;
#endif // _MSC_VER
}

// EOF
