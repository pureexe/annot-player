// qtwin.cc
// 7/21/2011

#include "qtwin/qtwin.h"
#include "qtwin/winreg.h"
#include <QtCore>
#include <cstdlib>
#include <memory>
#include <string>

#include <qt_windows.h>
#include <Psapi.h>
#include <ShellAPI.h>
#include <ShlGuid.h>
#include <ShlObj.h>
#include <ShObjIdl.h>
//#include <Shlwapi.h>
#include <TlHelp32.h>
//#include <strsafe.h>

//#ifdef _MSC_VER
//# pragma warning (disable: 4996)     // C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
//#endif // _MSC_VER

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
# define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
# define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif // _MSC_VER

// - Helpers -

#define MAKEDWORD(_a, _b)       ((DWORD)(((WORD)(((DWORD_PTR)(_a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(_b)) & 0xffff))) << 16))

#define QSTRING_LPCSTR(_qs)     (_qs).toStdString().c_str()
#define QSTRING_LPCWSTR(_qs)    (_qs).toStdWString().c_str()

namespace { namespace detail {

  typedef struct tag_HWND_DWORD {
    HWND hwnd;
    DWORD dword;
  } HWND_DWORD, *PHWND_DWORD;

  // Conversions between qt and win32 data structures

  inline QPoint POINT2QPoint(const POINT &pt)
  { return QPoint(pt.x, pt.y); }

  inline POINT QPoint2POINT(const QPoint &pos)
  { POINT ret = { pos.x(), pos.y() }; return ret; }

  inline QRect RECT2QRect(const RECT &rect)
  { return QRect(rect.left, rect.top, int(rect.right - rect.left), int(rect.bottom - rect.top)); } // QRect(x, y, w, h)

  inline RECT QRect2RECT(const QRect &r)
  { RECT ret = { r.x(), r.y(), r.right(), r.bottom() }; return ret; } // RECT { x1, y1, x2, y2 }

  // Return ached OS version info
  // See: http://msdn.microsoft.com/en-us/library/ms724451(v=VS.85).aspx
  LPOSVERSIONINFO lposvi()
  {
    static std::unique_ptr<OSVERSIONINFO> autoReleasePool;
    LPOSVERSIONINFO ret = autoReleasePool.get();
    if (!ret) {
      ret = new OSVERSIONINFO;
      ::ZeroMemory(ret, sizeof(*ret));
      ret->dwOSVersionInfoSize = sizeof(*ret);
      ::GetVersionEx(ret);

      autoReleasePool.reset(ret);
    }
    Q_ASSERT(ret);
    return ret;
  }

} } // anonymous detail

// - Maintence -

void
QtWin::warmUp()
{
  // Load cached functions
  isWindowsXpOrLater();
  isWindowsVistaOrLater();
}

// - Threads and processes -

bool
QtWin::isProcessActiveWithId(DWORD dwProcessId)
{
  bool ret = false;
  HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
  if (hProcess) {
    DWORD dwExitCode;
    ret = ::GetExitCodeProcess(hProcess, &dwExitCode) && (dwExitCode == STILL_ACTIVE);
    ::CloseHandle(hProcess);
  }
  return ret;
}

// See: http://hackdiy.com/I-11408248.html
// Requiring Psapi.lib.
QList<DWORD>
QtWin::getThreadIdsByProcessId(DWORD dwOwnerPID)
{
  QList<DWORD> ret;

  // Take a snapshot of all threads currently in the system.
  HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return ret;

  THREADENTRY32 te32 = { };
  te32.dwSize = sizeof(THREADENTRY32); // Fill in the size of the structure before using it.

  // Walk   the   hread snapshot to find all threads of the process.
  // If the thread belongs to the process, add its information
  // to the display list.
  if (::Thread32First(hSnapshot, &te32)) {
    do {
      if (te32.th32OwnerProcessID == dwOwnerPID)
        // te32.th32ThreadID          TID
        // te32.th32OwnerProcessID    Owner PID
        // te32.tpDeltaPri            Delta Priority
        // te32.tpBasePri             Base Priority
        ret.push_back(te32.th32ThreadID);
     } while (::Thread32Next(hSnapshot, &te32));
  } else // could not walk the list of threads
    // Do not forget to clean up the snapshot object.
    ::CloseHandle(hSnapshot);
  return ret;
}

QList<DWORD>
QtWin::getProcessIdsByParentProcessId(DWORD dwOwnerPID)
{
  QList<DWORD> ret;
  HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return ret;

  PROCESSENTRY32 pe = { };
  pe.dwSize = sizeof(pe); // Fill in the size of the structure before using it.

  if (::Process32First(hSnapshot, &pe)) {
    do {
      HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
      if(hProcess && pe.th32ParentProcessID == dwOwnerPID)
         ret.push_back(pe.th32ProcessID);
      ::CloseHandle(hProcess);
    } while(::Process32Next(hSnapshot, &pe));
  } else
    ::CloseHandle(hSnapshot);
  return ret;
}

QList<DWORD>
QtWin::getProcessIdsByName(const QString &processName)
{
  QList<DWORD> ret;
  HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return ret;

  PROCESSENTRY32 pe = { };
  pe.dwSize = sizeof(pe); // Fill in the size of the structure before using it.

  if (::Process32First(hSnapshot, &pe)) {
    do {
      HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
      if (hProcess &&
          processName == QString::fromWCharArray(pe.szExeFile))
        ret.append(pe.th32ProcessID);

      ::CloseHandle(hProcess);
    } while(::Process32Next(hSnapshot, &pe));
  } else
    ::CloseHandle(hSnapshot);
  return ret;
}

DWORD
QtWin::getProcessIdByName(const QString &processName)
{
  DWORD ret = 0; // either dwProcessId or 0
  HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return ret;

  PROCESSENTRY32 pe = { };
  pe.dwSize = sizeof(pe); // Fill in the size of the structure before using it.

  if (::Process32First(hSnapshot, &pe)) {
    do {
      HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
      if (hProcess &&
          processName == QString::fromWCharArray(pe.szExeFile))
        ret = pe.th32ProcessID;

      ::CloseHandle(hProcess);
    } while(!ret && ::Process32Next(hSnapshot, &pe));
  } else
    ::CloseHandle(hSnapshot);
  return ret;
}

QList<QtWin::ProcessInfo>
QtWin::getProcessesInfo()
{
  QList<ProcessInfo> ret;
  HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return ret;

  PROCESSENTRY32 pe = { };
  pe.dwSize = sizeof(pe); // Fill in the size of the structure before using it.

  if (::Process32First(hSnapshot, &pe)) {
    do {
      HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
      if (hProcess) {
        ProcessInfo pi;
        pi.id = pe.th32ProcessID;
        pi.name = QString::fromWCharArray(pe.szExeFile);
        ret.push_back(pi);
      }
      ::CloseHandle(hProcess);
    } while(::Process32Next(hSnapshot, &pe));
  } else
    ::CloseHandle(hSnapshot);
  return ret;
}

QString
QtWin::getProcessPathById(DWORD dwProcessId)
{
  QString ret;
  TCHAR szFilePath[MAX_PATH];
  HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId) ;
  if (hProcess) {
    int len = ::GetModuleFileNameEx(hProcess, NULL, szFilePath, MAX_PATH);
    if (len)
      ret = QString::fromWCharArray(szFilePath, len);
    ::CloseHandle(hProcess);
  }
  return ret;
}

QList<QtWin::ModuleInfo>
QtWin::getModulesInfo()
{
  QList<ModuleInfo> ret;
  HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return ret;

  MODULEENTRY32 me = { };
  me.dwSize = sizeof(me); // Fill in the size of the structure before using it.

  if (::Module32First(hSnapshot, &me)) {
    do {
      ModuleInfo mi;
      mi.moduleId = me.th32ModuleID;
      mi.processId = me.th32ProcessID;
      mi.moduleName = QString::fromWCharArray(me.szModule);
      mi.modulePath = QString::fromWCharArray(me.szExePath);
      ret.push_back(mi);
    } while(::Module32Next(hSnapshot, &me));
  } else
    ::CloseHandle(hSnapshot);
  return ret;
}

bool
QtWin::createProcess(const QString &filePath, const QStringList &env)
{
  std::wstring wszExePath = QDir::toNativeSeparators(filePath).toStdWString();

  QString fileDir = QFileInfo(filePath).absolutePath();
  std::wstring wszExeDir = QDir::toNativeSeparators(fileDir).toStdWString();

  STARTUPINFOW siStartupInfo;
  ::ZeroMemory(&siStartupInfo, sizeof(siStartupInfo));
  siStartupInfo.cb = sizeof(siStartupInfo);

  PROCESS_INFORMATION piProcessInfo;
  ::ZeroMemory(&piProcessInfo, sizeof(piProcessInfo));

  LPCWSTR lpAppName = wszExePath.c_str();
  LPWSTR pwszParam = nullptr;
  LPCWSTR lpCurrentDirectory = wszExeDir.c_str();

  LPVOID lpEnvironment = nullptr;
  if (!env.isEmpty()) {
    // See: http://stackoverflow.com/questions/4168369/how-does-a-unicode-environment-block-look-like-to-start-a-new-process-via-create
    // Example: WCHAR env[] = L"key=value\0key2=value2\0\0";
    size_t sz = 0;
    foreach (const QString &e, env)
      sz += e.size();
    if (sz) {
      sz += env.size() + 1;
      wchar_t *ws = new wchar_t[sz];
      size_t cur = 0;
      foreach (const QString &e, env)
        if (!e.isEmpty()) {
          int len = e.toWCharArray(ws + cur);
          ws[cur += len] = 0;
          cur++;
        }
      ws[cur] = 0;
      lpEnvironment = ws;
    }
  }

  DWORD dwCreationFlags = CREATE_DEFAULT_ERROR_MODE;
  if (lpEnvironment)
    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;

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
    lpAppName,          // app path
    pwszParam,          // app param
    0, 0,               // security attributes
    false,              // inherited
    dwCreationFlags,
    lpEnvironment,
    lpCurrentDirectory,
    &siStartupInfo,
    &piProcessInfo
  );

  if (lpEnvironment)
    delete lpEnvironment;
  return bResult;
}

void
QtWin::killCurrentProcess()
{
  enum { ExitCode = 0 };
  ::TerminateProcess(::GetCurrentProcess(), ExitCode);
}

// - Windows -

bool
QtWin::setFocus(QWidget *w)
{
  if (!w)
    return false;

  bool succeed = ::SetFocus(w->winId());
  w->setFocus();
  return succeed;
}

bool
QtWin::setFocus(HWND hwnd)
{ return hwnd && ::SetFocus(hwnd); }

void
QtWin::repaintWindow(HWND hwnd)
{ ::InvalidateRect(hwnd, NULL, TRUE); }

HWND
QtWin::getChildWindow(HWND hwnd)
{
  if (!hwnd)
    return 0;
  HWND child = ::GetWindow(hwnd, GW_CHILD);
  return child && ::IsWindow(child) ? child : 0;
}

QList<HWND>
QtWin::getChildWindows(HWND hwnd)
{
  QList<HWND> ret;
  HWND h = hwnd;
  if (h)
    while (h = QtWin::getChildWindow(h))
      ret.append(h);
  return ret;
}

HWND
QtWin::getWindowAtPos(const QPoint &globalPos)
{ return ::WindowFromPoint(detail::QPoint2POINT(globalPos)); }

HWND
QtWin::getChildWindowAtPos(const QPoint &globalPos, HWND parent)
{ return ::ChildWindowFromPoint(parent, detail::QPoint2POINT(globalPos)); }

QRect
QtWin::getWindowRect(HWND hwnd)
{
  QRect ret;
  RECT rect;
  if (::GetWindowRect(hwnd, &rect))
    ret = detail::RECT2QRect(rect);
  return ret;
}

bool
QtWin::windowHasRect(WId hwnd)
{
  RECT rect;
  return ::GetWindowRect(hwnd, &rect) &&
      rect.right > rect.left &&
      rect.bottom > rect.top;
}

bool
QtWin::isGoodWindow(WId hwnd)
{
  return windowHasText(hwnd) && (
    isWindowVisible(hwnd) && windowHasRect(hwnd) ||
    isWindowMinimized(hwnd)
  );
}

bool
QtWin::isWindowValid(HWND hwnd)
{ return ::IsWindow(hwnd); }

bool
QtWin::isWindowVisible(WId hwnd)
{ return ::IsWindowVisible(hwnd); }

bool
QtWin::isWindowMinimized(WId hwnd)
{ return ::IsIconic(hwnd); }

bool
QtWin::setTopWindow(HWND hwnd)
{ return ::BringWindowToTop(hwnd); }

HWND
QtWin::getTopWindow(HWND hwnd)
{ return ::GetTopWindow(hwnd); }

HWND
QtWin::getPreviousWindow(HWND hwnd)
{ return ::GetNextWindow(hwnd, GW_HWNDPREV); }

HWND
QtWin::getNextWindow(HWND hwnd)
{ return ::GetNextWindow(hwnd, GW_HWNDNEXT); }

bool
QtWin::isWindowAboveWindow(HWND parent, HWND target)
{
  HWND next = parent;
  while (next = ::GetNextWindow(next, GW_HWNDNEXT))
    if (next == target)
      return true;
  return false;
}

bool
QtWin::isWindowBelowWindow(HWND parent, HWND target)
{
  HWND next = parent;
  while (next = ::GetNextWindow(next, GW_HWNDPREV))
    if (next == target)
      return true;
  return false;
}

bool
QtWin::windowHasText(WId hwnd)
{
  wchar_t wsz[2];
  int count = ::GetWindowTextW(hwnd, wsz, sizeof(wsz)/sizeof(*wsz));
  return count && wsz[0];
}

QString
QtWin::getWindowText(WId hwnd, size_t size)
{
  enum { MAX_SIZE = MAX_PATH + 1 };
  QString ret;
  if (!hwnd)
    return ret;
  if (size) {
    wchar_t *wsz = new wchar_t[size];
    size = ::GetWindowTextW(hwnd, wsz, size);
    if (size)
      ret = QString::fromWCharArray(wsz, size);
    delete wsz;
  } else {
    wchar_t wsz[MAX_SIZE];
    size = ::GetWindowTextW(hwnd, wsz, sizeof(wsz)/sizeof(*wsz));
    if (size)
      ret = QString::fromWCharArray(wsz, size);
  }
  return ret;
}

DWORD
QtWin::getWindowThreadId(HWND hwnd)
{ return ::GetWindowThreadProcessId(hwnd, 0); }

DWORD
QtWin::getWindowProcessId(HWND hwnd)
{
  DWORD dwProcessId;
  ::GetWindowThreadProcessId(hwnd, &dwProcessId);
  return dwProcessId;
}

namespace { // anonymous

  BOOL CALLBACK
  getWindowWithThreadId_p(HWND hwnd, LPARAM lparam)
  {
    auto lp = reinterpret_cast<detail::PHWND_DWORD>(lparam);
    Q_ASSERT(lp);
    if (QtWin::getWindowThreadId(hwnd) == lp->dword) {
      lp->hwnd = hwnd;
      return false; // break enumeration
    } else
      return true; // continue enumeration
  }

  BOOL CALLBACK
  getVisibleWindowWithThreadId_p(HWND hwnd, LPARAM lparam)
  {
    auto lp = reinterpret_cast<detail::PHWND_DWORD>(lparam);
    Q_ASSERT(lp);
    if (::IsWindowVisible(hwnd) &&
        QtWin::getWindowThreadId(hwnd) == lp->dword) {
      lp->hwnd = hwnd;
      return false; // break enumeration
    } else
      return true; // continue enumeration
  }

  BOOL CALLBACK
  getGoodWindowWithThreadId_p(HWND hwnd, LPARAM lparam)
  {
    auto lp = reinterpret_cast<detail::PHWND_DWORD>(lparam);
    Q_ASSERT(lp);
    if (QtWin::getWindowThreadId(hwnd) == lp->dword &&
        QtWin::isGoodWindow(hwnd)) {
      lp->hwnd = hwnd;
      return false; // break enumeration
    } else
      return true; // continue enumeration
  }

  BOOL CALLBACK
  getWindowWithProcessId_p(HWND hwnd, LPARAM lparam)
  {
    auto lp = reinterpret_cast<detail::PHWND_DWORD>(lparam);
    Q_ASSERT(lp);
    if (QtWin::getWindowProcessId(hwnd) == lp->dword) {
      lp->hwnd = hwnd;
      return false; // break enumeration
    } else
      return true; // continue enumeration
  }

  BOOL CALLBACK
  getVisibleWindowWithProcessId_p(HWND hwnd, LPARAM lparam)
  {
    auto lp = reinterpret_cast<detail::PHWND_DWORD>(lparam);
    Q_ASSERT(lp);
    if (::IsWindowVisible(hwnd) &&
        QtWin::getWindowProcessId(hwnd) == lp->dword) {
      lp->hwnd = hwnd;
      return false; // break enumeration
    } else
      return true; // continue enumeration
  }

  BOOL CALLBACK
  getGoodWindowWithProcessId_p(HWND hwnd, LPARAM lparam)
  {
    auto lp = reinterpret_cast<detail::PHWND_DWORD>(lparam);
    Q_ASSERT(lp);
    if (QtWin::getWindowProcessId(hwnd) == lp->dword &&
        QtWin::isGoodWindow(hwnd)) {
      lp->hwnd = hwnd;
      return false; // break enumeration
    } else
      return true; // continue enumeration
  }
} // anonymous namespace

HWND
QtWin::getWindowWithThreadId(DWORD threadId)
{
  detail::HWND_DWORD tuple;
  tuple.hwnd = nullptr;
  tuple.dword = threadId;
  ::EnumWindows(::getWindowWithThreadId_p, reinterpret_cast<LPARAM>(&tuple));
  return tuple.hwnd;
}

HWND
QtWin::getWindowWithProcessId(DWORD processId)
{
  detail::HWND_DWORD tuple;
  tuple.hwnd = nullptr;
  tuple.dword = processId;
  ::EnumWindows(::getWindowWithProcessId_p, reinterpret_cast<LPARAM>(&tuple));
  return tuple.hwnd;
}

HWND
QtWin::getVisibleWindowWithThreadId(DWORD threadId)
{
  detail::HWND_DWORD tuple;
  tuple.hwnd = nullptr;
  tuple.dword = threadId;
  ::EnumWindows(::getVisibleWindowWithThreadId_p, reinterpret_cast<LPARAM>(&tuple));
  return tuple.hwnd;
}

HWND
QtWin::getVisibleWindowWithProcessId(DWORD processId)
{
  detail::HWND_DWORD tuple;
  tuple.hwnd = nullptr;
  tuple.dword = processId;
  ::EnumWindows(::getVisibleWindowWithProcessId_p, reinterpret_cast<LPARAM>(&tuple));
  return tuple.hwnd;
}

HWND
QtWin::getGoodWindowWithThreadId(DWORD threadId)
{
  detail::HWND_DWORD tuple;
  tuple.hwnd = nullptr;
  tuple.dword = threadId;
  ::EnumWindows(::getGoodWindowWithThreadId_p, reinterpret_cast<LPARAM>(&tuple));
  return tuple.hwnd;
}

HWND
QtWin::getGoodWindowWithProcessId(DWORD processId)
{
  detail::HWND_DWORD tuple;
  tuple.hwnd = nullptr;
  tuple.dword = processId;
  ::EnumWindows(::getGoodWindowWithProcessId_p, reinterpret_cast<LPARAM>(&tuple));
  return tuple.hwnd;
}

//QList<HWND>
//QtWin::getWindowsWithThreadId(DWORD threadId)
//{ return QList<HWND>(); }
//
//QList<HWND>
//QtWin::getWindowsWithProcessId(DWORD processId)
//{ return QList<HWND>(); }


// - Mouse and keyboard -

// Note: GetAsyncKeyState does not work
bool
QtWin::isKeyPressed(int vk)
{ return ::GetKeyState(vk) & 0xF0; }

bool
QtWin::isKeyToggled(int vk)
{ return ::GetKeyState(vk) & 0x0F; }

bool
QtWin::isKeyCapslockToggled()
{ return isKeyToggled(VK_CAPITAL); }

bool
QtWin::isKeyShiftPressed()
{ return isKeyPressed(VK_SHIFT); }

bool
QtWin::isKeyAltPressed()
{ return isKeyPressed(VK_MENU); }

bool
QtWin::isKeyControlPressed()
{ return isKeyPressed(VK_CONTROL); }

bool
QtWin::isKeyWinPressed()
{ return isKeyPressed(VK_LWIN) || isKeyPressed(VK_RWIN); }

bool
QtWin::isMouseLeftButtonPressed()
{ return isKeyPressed(VK_LBUTTON); }

bool
QtWin::isMouseMiddleButtonPressed()
{ return isKeyPressed(VK_MBUTTON); }

bool
QtWin::isMouseRightButtonPressed()
{ return isKeyPressed(VK_RBUTTON); }

int
QtWin::getDoubleClickInterval()
{
  static const int ret = ::GetDoubleClickTime();
  return ret;
}

void
QtWin::sendMouseMove(const QPoint &globalPos, bool relative)
{
  DWORD f = relative ? 0 : MOUSEEVENTF_ABSOLUTE;
  ::mouse_event(f | MOUSEEVENTF_MOVE, globalPos.x(), globalPos.y(), 0, 0);
}

// See mouse_event: http://msdn.microsoft.com/en-us/library/ms646260(v=vs.85).aspx
void
QtWin::sendMouseClick(const QPoint& globalPos, Qt::MouseButton button, bool relative)
{
  DWORD f = relative ? 0 : MOUSEEVENTF_ABSOLUTE;
  switch (button) {
  case Qt::LeftButton:
    ::mouse_event(f | MOUSEEVENTF_LEFTDOWN,     globalPos.x(), globalPos.y(), 0, 0);
    ::mouse_event(f | MOUSEEVENTF_LEFTUP,       globalPos.x(), globalPos.y(), 0, 0);
    break;
  case Qt::RightButton:
    ::mouse_event(f | MOUSEEVENTF_RIGHTDOWN,    globalPos.x(), globalPos.y(), 0, 0);
    ::mouse_event(f | MOUSEEVENTF_RIGHTUP,      globalPos.x(), globalPos.y(), 0, 0);
    break;
  case Qt::MiddleButton:
    ::mouse_event(f | MOUSEEVENTF_MIDDLEDOWN,   globalPos.x(), globalPos.y(), 0, 0);
    ::mouse_event(f | MOUSEEVENTF_MIDDLEUP,     globalPos.x(), globalPos.y(), 0, 0);
    break;
  case Qt::XButton1:
  case Qt::XButton2:
    ::mouse_event(f | MOUSEEVENTF_XDOWN,        globalPos.x(), globalPos.y(), 0, 0);
    ::mouse_event(f | MOUSEEVENTF_XUP,          globalPos.x(), globalPos.y(), 0, 0);
    break;
  }
}

QPoint
QtWin::getMousePos()
{
  POINT pt;
  return ::GetCursorPos(&pt) ? detail::POINT2QPoint(pt) : QPoint();
}

// - Paths -
// It is suggested to use SHGetFolderPath or SHGetKnownFolderPath, which is not available to Windows XP

QString
QtWin::getPath()
{ return qgetenv("PATH"); }

QString
QtWin::getWinDirPath()
{  return qgetenv("windir");  }

QString
QtWin::getSystemRoot()
{ return qgetenv("SYSTEMROOT");  }

QString
QtWin::getProgramFilesPath()
{ return qgetenv("PROGRAMFILES"); }

QString
QtWin::getAppDataPath()
{ return qgetenv("APPDATA"); }

QString
QtWin::getLocalAppDataPath()
{ return qgetenv("LOCALAPPDATA"); }

QString
QtWin::getFontsPath()
{
  return QSettings(REG_HKCU_SHELLFOLDERS, QSettings::NativeFormat)
      .value("Fonts").toString();
}

QString
QtWin::getMusicPath()
{
  return QSettings(REG_HKCU_SHELLFOLDERS, QSettings::NativeFormat)
      .value("My Music").toString();
}

QString
QtWin::getVideoPath()
{
  return QSettings(REG_HKCU_SHELLFOLDERS, QSettings::NativeFormat)
      .value("My Video").toString();
}

QString
QtWin::getDocumentsPath()
{
  return QSettings(REG_HKCU_SHELLFOLDERS, QSettings::NativeFormat)
      .value("Personal").toString();
}

// See: http://msdn.microsoft.com/en-us/library/windows/desktop/bb762188(v=vs.85).aspx
// See: http://msdn.microsoft.com/en-us/library/windows/desktop/dd378457(v=vs.85).aspx
// Not implemeneted since SHGetKnownFolderPath is not available to Windows XP.
QString
QtWin::getDownloadsPath()
{ return QDir::homePath() + "/" "Downloads"; }

// - System information -

// See: http://msdn.microsoft.com/en-us/library/ms724451(v=VS.85).aspx
// See: http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
bool
QtWin::isWindowsXpOrLater()
{
  static int ret = -1;
  if (ret < 0) {
    LPOSVERSIONINFO lp = detail::lposvi();
    ret = (lp->dwMajorVersion > 5 ||
           lp->dwMajorVersion == 5 && lp->dwMinorVersion >= 1) // dwMinorVersion == 0 => Windows 2000
        ? 1 : 0;
  }
  return ret;
}

bool
QtWin::setPath(const QString &path)
{ return qputenv("PATH", path.toLocal8Bit()); }

bool
QtWin::prependPath(const QString &path)
{
  if (path.isEmpty())
    return true;
  QString winpath = QDir::toNativeSeparators(path);
  QString value = getPath();
  if (value.isEmpty())
    value = winpath;
  else
    value.prepend(';').prepend(winpath);
  return setPath(value);
}

bool
QtWin::appendPath(const QString &path)
{
  if (path.isEmpty())
    return true;
  QString winpath = QDir::toNativeSeparators(path);
  QString value = getPath();
  if (value.isEmpty())
    value = winpath;
  else if (value.endsWith(';'))
    value.append(winpath);
  else
    value.append(';').append(winpath);
  return setPath(value);
}

bool
QtWin::isWindowsVistaOrLater()
{
  static int ret = -1;
  if (ret < 0)
    ret = detail::lposvi()->dwMajorVersion >= 6 ? 1 : 0;
  return ret;
}

// - Files -

// Requiring Ole32.dll.
bool
QtWin::createLink(const QString &lnkPath, const QString &targetPath, const QString &description)
{
  if (lnkPath.isEmpty() || targetPath.isEmpty())
    return false;

  std::wstring wszLnkPath = QDir::toNativeSeparators(lnkPath).toStdWString(),
               wszTargetPath = QDir::toNativeSeparators(targetPath).toStdWString(),
               wszDescription = QDir::toNativeSeparators(description).toStdWString();

  // See: http://msdn.microsoft.com/en-us/library/bb776891(VS.85).aspx
  //
  // CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces
  //              to create and store a shortcut to the specified object.
  //
  // Returns the result of calling the member functions of the interfaces.
  //
  // Parameters:
  // lpszPathObj  - Address of a buffer that contains the path of the object,
  //                including the file name.
  // lpszPathLink - Address of a buffer that contains the path where the
  //                Shell link is to be stored, including the file name.
  // lpszDesc     - Address of a buffer that contains a description of the
  //                Shell link, stored in the Comment field of the link
  //                properties.
  LPCWSTR lpszPathLink = wszLnkPath.c_str(),
          lpszPathObj = wszTargetPath.c_str(),
          lpszDesc = wszDescription.empty() ? 0 : wszDescription.c_str();

  HRESULT hres;
  IShellLink *psl;

  // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
  // has already been called.
  hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
  if (SUCCEEDED(hres))  {
    IPersistFile* ppf;
    // Set the path to the shortcut target and add the description.
    psl->SetPath(lpszPathObj);
    if (lpszDesc)
      psl->SetDescription(lpszDesc);

    // Query IShellLink for the IPersistFile interface, used for saving the
    // shortcut in persistent storage.
    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
    if (SUCCEEDED(hres))  {

      // Ensure that the string is Unicode.
      //WCHAR wsz[MAX_PATH];
      //::MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);

      // Add code here to check return value from MultiByteWideChar
      // for success.

      // Save the link by calling IPersistFile::Save.
      hres = ppf->Save(lpszPathLink, TRUE);
      ppf->Release();
    }
    psl->Release();
  }
  return SUCCEEDED(hres);
}

// Requiring Ole32.dll.
QString
QtWin::resolveLink(const QString &lnkPath, HWND hwnd)
{
  QString ret;
  if (lnkPath.isEmpty())
    return ret;

  std::wstring wszLnkPath = QDir::toNativeSeparators(lnkPath).toStdWString();
  //WCHAR wszRet[MAX_PATH];

  // ResolveIt - Uses the Shell's IShellLink and IPersistFile interfaces
  //             to retrieve the path and description from an existing shortcut.
  //
  // Returns the result of calling the member functions of the interfaces.
  //
  // Parameters:
  // hwnd         - A handle to the parent window. The Shell uses this window to
  //                display a dialog box if it needs to prompt the user for more
  //                information while resolving the link.
  // lpszLinkFile - Address of a buffer that contains the path of the link,
  //                including the file name.
  // lpszPath     - Address of a buffer that receives the path of the link
  //                target, including the file name.
  // lpszDesc     - Address of a buffer that receives the description of the
  //                Shell link, stored in the Comment field of the link
  //                properties.
  LPCWSTR lpszLinkFile = wszLnkPath.c_str();
  //LPWSTR lpszPath = wszRet;
  //int iPathBufferSize = sizeof(wszRet) / sizeof(*wszRet);

  HRESULT hres;
  IShellLink *psl;
  WCHAR szGotPath[MAX_PATH];
  //WCHAR szDescription[MAX_PATH];
  WIN32_FIND_DATA wfd;

  szGotPath[0] = '\0';
  //*lpszPath = '\0'; // Assume failure

  // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
  // has already been called.
  hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
  if (SUCCEEDED(hres))  {
    IPersistFile* ppf;

    // Get a pointer to the IPersistFile interface.
    hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

    if (SUCCEEDED(hres))  {

      // Ensure that the string is Unicode.
      //WCHAR wsz[MAX_PATH];
      //::MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH);

     // Add code here to check return value from MultiByteWideChar
     // for success.

     // Load the shortcut.
     hres = ppf->Load(lpszLinkFile, STGM_READ);
     if (SUCCEEDED(hres))  {
       // Resolve the link.
       if (hwnd)
         hres = psl->Resolve((HWND)hwnd, 0);
       if (SUCCEEDED(hres))  {
         // Get the path to the link target.
         hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);
         /*
         if (SUCCEEDED(hres))  {
           // Get the description of the target.
           hres = psl->GetDescription(szDescription, MAX_PATH);

           if (SUCCEEDED(hres))  {
             hres = ::StringCbCopy(lpszPath, iPathBufferSize, szGotPath);
             if (SUCCEEDED(hres)) {
                 // Handle success
              } else {
                // Handle the error
              }
            }
          }
          */
        }
      }

      // Release the pointer to the IPersistFile interface.
      ppf->Release();
    }

    // Release the pointer to the IShellLink interface.
    psl->Release();
  }

  if (SUCCEEDED(hres))
    ret = QString::fromWCharArray(szGotPath);
  return ret;
}

bool
QtWin::trashFile(const QString &fileName, bool confirm, WId hwnd)
{
  enum { BufferSize = MAX_PATH + 1 };
  QString filePath = QDir::toNativeSeparators(fileName);
  if (BufferSize < filePath.size())
    return false;
  //std::wstring wszFilePath = filePath.toStdWString();
  wchar_t wszFilePath[BufferSize];
  filePath.toWCharArray(wszFilePath);
  wszFilePath[filePath.size()] = L'\0';

  SHFILEOPSTRUCT op = { };
  op.hwnd = hwnd;
  op.wFunc = FO_DELETE;
  //op.pTo = nullptr;
  //op.pFrom = const_cast<PWSTR>(wszFilePath.c_str());
  op.pFrom = wszFilePath;
  op.fFlags = confirm ? FOF_ALLOWUNDO : FOF_ALLOWUNDO|FOF_NOCONFIRMATION;
  int err = SHFileOperationW(&op);
  //qDebug() << QString::fromStdWString(wszFilePath) << QString::number(err, 16);
  return !err;
}

bool
QtWin::isFileExists(const QString &fileName)
{
#ifdef _INC_SHLWAPI // Require Shlwapi.sll
  return ::PathFileExistsW(QSTRING_LPCWSTR(QDir::toNativeSeparators(fileName)));
#else
  return QFile::exists(fileName);
#endif // _INC_SHLWAPI
}

bool
QtWin::deleteFile(const QString &fileName)
{ return ::DeleteFileW(QSTRING_LPCWSTR(QDir::toNativeSeparators(fileName))); }

// - Devices -

// See: http://www.codeguru.com/forum/archive/index.php/t-252784.html
QStringList
QtWin::getLogicalDrives()
{
  QStringList ret;

  enum { BufSize = 3 * 26 }; // At most 26 drives, each take 3 chars such as "C:\"
  char szBuffer[BufSize];

  DWORD dwReturnSize = ::GetLogicalDriveStringsA(BufSize, szBuffer);
  if (dwReturnSize) {
    char *pch = szBuffer;
    while (*pch) {
      ret.append(pch);
      pch += qstrlen(pch) + 1;
    }
  }

  return ret;
}

QtWin::DriveType
QtWin::getDriveType(const QString &driveLetter)
{
  return (DriveType)
      ::GetDriveTypeA(driveLetter.toAscii());
}

QStringList
QtWin::getLogicalDrivesWithType(DriveType type)
{
  QStringList ret;

  enum { BufSize = 3 * 26 }; // At most 26 drives, each take 3 chars such as "C:\"
  char szBuffer[BufSize];

  DWORD dwReturnSize = ::GetLogicalDriveStringsA(BufSize, szBuffer);
  if (dwReturnSize) {
    char *pch = szBuffer;
    while (*pch) {
      if (::GetDriveTypeA(pch) == type)
        ret.append(pch);
      pch += qstrlen(pch) + 1;
    }
  }

  return ret;
}

char
QtWin::getFileDriveLetter(const QString &fileName)
{
  if (fileName.size() < 2)
    return 0;
  if (fileName[1] == ':') {
    QChar ret = fileName[0];
    if (ret.isLetter())
      return ret.toAscii();
  } else if (fileName.startsWith("file://", Qt::CaseInsensitive)) {
    QRegExp rx("^file://[/]*([a-z]):", Qt::CaseInsensitive);
    if (rx.indexIn(fileName) >= 0)
      return rx.cap(1)[0].toAscii();
  }
  return 0;
}

QString
QtWin::guessDeviceFileName(const QString &hint)
{
  if (hint.isEmpty())
    return QString();

  QString normalized = hint.trimmed().toUpper();
  if (normalized.isEmpty())
    return QString();

  else if (normalized.contains(QRegExp("^\\\\\\\\\\.\\\\[A-Z]:$")))
    return normalized;
  else if (normalized.contains(QRegExp("^[A-Z]:$")))
    return "\\\\.\\" + normalized;
  else if (normalized.contains(QRegExp("^[A-Z]:\\\\$"))) {
    normalized.chop(1);
    return "\\\\.\\" + normalized;
  }
  else
    return QString();
}

bool
QtWin::isValidDeviceFileName(const QString &fileName)
{ return fileName.contains(QRegExp("^\\\\\\\\\\.\\\\[A-Z]:$", Qt::CaseInsensitive)); }

#if 0
// Require Winmm.lib.
bool
QtWin::setWaveVolume(qreal percentage)
{
  enum { MAX_VOLUME = 0xff };
  if (percentage < 0)
    percentage = 0;
  else if (percentage > 1)
    percentage = 1;

  WORD w = MAX_VOLUME * percentage;
  DWORD dw = MAKEDWORD(w, w);

  return ::waveOutSetVolume(0, dw) == MMSYSERR_NOERROR;
}

// Require Winmm.lib.
qreal
QtWin::getWaveVolume()
{
  enum { MAX_VOLUME = 0xff };

  qreal ret = -1;
  DWORD dwVolume;
  if (::waveOutGetVolume(0, &dwVolume) == MMSYSERR_NOERROR)
    ret = (LOWORD(dwVolume) + HIWORD(dwVolume)) / (2.0 * MAX_VOLUME);

  return ret;
}

#endif // 0

// - Run -

bool
QtWin::run(const QString &cmd, bool visible)
{
  UINT uCmdShow = visible ? SW_SHOW : SW_HIDE;
  return ::WinExec(cmd.toLocal8Bit(), uCmdShow) > 31;
}

bool
QtWin::halt()
{ return run("shutdown -s", false); } // visible = false

bool
QtWin::reboot()
{ return run("shutdown -t", false); } // visible = false

bool
QtWin::logoff()
{ return run("shutdown -l", false); } // visible = false

// Standby %windir%\System32\rundll32.exe powrprof.dll,SetSuspendState Standby
// Hibernate %windir%\System32\rundll32.exe powrprof.dll,SetSuspendState Hibernate
bool
QtWin::hibernate()
{ return run("shutdown -h", false); } // visible = false

// - POSIX -

void
QtWin::sleep(long msecs)
{ ::Sleep(msecs); }

// - Encoding -

// See
const char *
QtWin::toUtf8(const wchar_t *pStr)
{
  static char szBuf[1024]; // not threadsafe!
  WideCharToMultiByte(CP_UTF8, 0, pStr, -1, szBuf, sizeof(szBuf), NULL, NULL);
  return szBuf;
}

// - File system -

ulong
QtWin::getFileAttributes(const QString &fileName)
{ return ::GetFileAttributesW(QSTRING_LPCWSTR(QDir::toNativeSeparators(fileName))); }

bool
QtWin::setFileAttributes(const QString &fileName, uint flags)
{
  return ::SetFileAttributesW(
        QSTRING_LPCWSTR(QDir::toNativeSeparators(fileName)),
        flags);
}

// - Shell -

// See: http://nicug.blogspot.com/search/label/taskbar
void
QtWin::addRecentDocument(const QString &fileName)
{
  ::SHAddToRecentDocs(
        SHARD_PATHW,
        QSTRING_LPCWSTR(QDir::toNativeSeparators(fileName)));
}

// EOF

/*

// See: http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/430449b3-f6dd-4e18-84de-eebd26a8d668/
int
QtWin::gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    // converting file time to unix epoch
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tmpres /= 10;  // convert into microseconds
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

// See: http://msdn.microsoft.com/en-us/library/bb776891(VS.85).aspx
//
// CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces
//              to create and store a shortcut to the specified object.
//
// Returns the result of calling the member functions of the interfaces.
//
// Parameters:
// lpszPathObj  - Address of a buffer that contains the path of the object,
//                including the file name.
// lpszPathLink - Address of a buffer that contains the path where the
//                Shell link is to be stored, including the file name.
// lpszDesc     - Address of a buffer that contains a description of the
//                Shell link, stored in the Comment field of the link
//                properties.

#include "stdafx.h"
#include "windows.h"
#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"

HRESULT CreateLink(LPCWSTR lpszPathObj, LPCSTR lpszPathLink, LPCWSTR lpszDesc)
{
    HRESULT hres;
    IShellLink* psl;

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Set the path to the shortcut target and add the description.
        psl->SetPath(lpszPathObj);
        psl->SetDescription(lpszDesc);

        // Query IShellLink for the IPersistFile interface, used for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH];

            // Ensure that the string is Unicode.
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);

            // Add code here to check return value from MultiByteWideChar
            // for success.

            // Save the link by calling IPersistFile::Save.
            hres = ppf->Save(wsz, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
}


// See: http://msdn.microsoft.com/en-us/library/bb776891(VS.85).aspx
//
// ResolveIt - Uses the Shell's IShellLink and IPersistFile interfaces
//             to retrieve the path and description from an existing shortcut.
//
// Returns the result of calling the member functions of the interfaces.
//
// Parameters:
// hwnd         - A handle to the parent window. The Shell uses this window to
//                display a dialog box if it needs to prompt the user for more
//                information while resolving the link.
// lpszLinkFile - Address of a buffer that contains the path of the link,
//                including the file name.
// lpszPath     - Address of a buffer that receives the path of the link
                  target, including the file name.
// lpszDesc     - Address of a buffer that receives the description of the
//                Shell link, stored in the Comment field of the link
//                properties.

#include "stdafx.h"
#include "windows.h"
#include "shobjidl.h"
#include "shlguid.h"
#include "strsafe.h"

HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, LPWSTR lpszPath, int iPathBufferSize)
{
    HRESULT hres;
    IShellLink* psl;
    WCHAR szGotPath[MAX_PATH];
    WCHAR szDescription[MAX_PATH];
    WIN32_FIND_DATA wfd;

    *lpszPath = nullptr; // Assume failure

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Get a pointer to the IPersistFile interface.
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH];

            // Ensure that the string is Unicode.
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH);

            // Add code here to check return value from MultiByteWideChar
            // for success.

            // Load the shortcut.
            hres = ppf->Load(wsz, STGM_READ);

            if (SUCCEEDED(hres))
            {
                // Resolve the link.
                hres = psl->Resolve(hwnd, 0);

                if (SUCCEEDED(hres))
                {
                    // Get the path to the link target.
                    hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);

                    if (SUCCEEDED(hres))
                    {
                        // Get the description of the target.
                        hres = psl->GetDescription(szDescription, MAX_PATH);

                        if (SUCCEEDED(hres))
                        {
                            hres = StringCbCopy(lpszPath, iPathBufferSize, szGotPath);
                            if (SUCCEEDED(hres))
                            {
                                // Handle success
                            }
                            else
                            {
                                // Handle the error
                            }
                        }
                    }
                }
            }

            // Release the pointer to the IPersistFile interface.
            ppf->Release();
        }

        // Release the pointer to the IShellLink interface.
        psl->Release();
    }
    return hres;
}

*/
