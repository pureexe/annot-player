// main.tcc
// 9/3/2011
#include "msc.h"
#include <windows.h>
#include <string>
#include <memory>

//#define DEBUG "main"
//#include "module/debug/debug.h"
//#ifdef DEBUG
//  #include <QtCore>
//  #define DOUT(_msg)    qDebug() << "main:" << _msg
//#else
  #define DOUT(_dummy)  (void)0
//#endif // DEBUG

// - Launcher -

namespace { // anonymous

  inline std::string
  dirname(const std::string &path)
  { return path.substr(0, path.find_last_of('\\')); }

  inline std::wstring
  dirname(const std::wstring &path)
  { return path.substr(0, path.find_last_of(L'\\')); }

} // anonymous namespace


// - Main -

int CALLBACK
WinMain(__in HINSTANCE hInstance, __in HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nCmdShow)
{
  UNUSED(hInstance);
  UNUSED(hPrevInstance);
  UNUSED(nCmdShow);

  enum { BUFFER_SIZE = MAX_PATH * 3 };
  WCHAR wszBuffer[BUFFER_SIZE]; {
    int nSize = ::GetModuleFileNameW(0, wszBuffer, BUFFER_SIZE);
    if (nSize == BUFFER_SIZE)
      return -1;
  }
  std::wstring wsDir = dirname(wszBuffer);
  std::wstring wsApp = wsDir + (L"\\" APP_PREFIX APP_EXE);
  std::wstring wsAppPath = dirname(wsApp);

  // See: http://msdn.microsoft.com/en-us/library/windows/desktop/cc144102(v=vs.85).aspx
  ::SetFileAttributesW(wsDir.c_str(), FILE_ATTRIBUTE_READONLY);

  DOUT(QString::fromStdWString(app));

  STARTUPINFOW siStartupInfo;
  ::memset(&siStartupInfo, 0, sizeof(siStartupInfo));
  siStartupInfo.cb = sizeof(siStartupInfo);

  PROCESS_INFORMATION piProcessInfo;
  ::memset(&piProcessInfo, 0, sizeof(piProcessInfo));

  LPVOID lpEnvironment = nullptr;     // TODO

  LPWSTR lpwCmdLine = nullptr;;
  if (lpCmdLine) {
    BOOL bResult = ::MultiByteToWideChar(CP_ACP, 0, lpCmdLine, -1, wszBuffer, BUFFER_SIZE);
    if (bResult)
      lpwCmdLine = wszBuffer;
  }
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
    wsApp.c_str(),      // app path
    lpwCmdLine,         // app param
    0, 0,               // security attributes
    false,              // inherited
    CREATE_DEFAULT_ERROR_MODE, // creation flags
    lpEnvironment,
    wsAppPath.c_str(),
    &siStartupInfo,
    &piProcessInfo
  );

  return bResult ? 0 : -1;
}

// EOF
