// main.tcc
// 9/3/2011
#include "global.h"
#include "util.h"
#include <windows.h>
#include <string>
#include <memory>

//#define DEBUG "main"
//#include "qtx/qxdebug.h"
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
  UNUSED(lpCmdLine);

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
  //::SetFileAttributesW(wsDir.c_str(), FILE_ATTRIBUTE_READONLY);

  DOUT(QString::fromStdWString(app));

  STARTUPINFOW siStartupInfo;
  ::memset(&siStartupInfo, 0, sizeof(siStartupInfo));
  siStartupInfo.cb = sizeof(siStartupInfo);

  PROCESS_INFORMATION piProcessInfo;
  ::memset(&piProcessInfo, 0, sizeof(piProcessInfo));

  //LPWSTR lpwCmdLine = nullptr;;
  //if (lpCmdLine) {
  //  BOOL bResult = ::MultiByteToWideChar(CP_ACP, 0, lpCmdLine, -1, wszBuffer, BUFFER_SIZE);
  //  if (bResult)
  //    lpwCmdLine = wszBuffer;
  //}

  // See: http://msdn.microsoft.com/ja-jp/library/cc422072.aspx
  //
  // BOOL WINAPI CreateProcess(
  // HINSTANCE ShellExecute(
  //   __in HWND hwnd,              // 親ウィンドウのハンドル
  //   __in LPCTSTR lpVerb,         // 操作
  //   __in LPCTSTR lpFile,         // 操作対象のファイル
  //   __in LPCTSTR lpParameters,   // 操作のパラメータ
  //   __in LPCTSTR lpDirectory,    // 既定のディレクトリ
  //   __in INT nShowCmd            // 表示状態
  // );
  HINSTANCE h = ::ShellExecuteW(
    nullptr,    // parent window
    L"open",
    wsApp.c_str(),
    nullptr,    // parameters
    wsAppPath.c_str(),
    nCmdShow
  );

  return h ? 0 : -1;
}

// EOF
