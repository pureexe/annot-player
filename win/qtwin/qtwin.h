#pragma once

// qtwin.h
// 7/21/2011

#include <QWidget>
#include <QList>
#include <QString>
#include <QPoint>
#include <QRect>

namespace QtWin {

  // Direct access to windows DLL module
  namespace Module { }

  // Direct access windows DLL api
  namespace Api { }

  // - POSIX -

  struct timezone
  {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
  };

  //int gettimeofday(struct timeval *tv, struct timezone *tz);

  void sleep(long msecs);

  // - Maintence -

  void warmUp(); ///< Not mandatory, but for better speed up

  // - Main module -

  ///  Return main module path as LPWSTR
  inline wchar_t *getModulePath()
  { __asm {
    mov eax,fs:[0x30]
    mov eax,[eax+0xC]
    mov eax,[eax+0xC]
    mov eax,[eax+0x28]
  } }

  ///  Return main module handle as HINSTANCE
  inline void *getModuleBase()
  { __asm {
    mov eax,fs:[0x30]
    mov eax,[eax+0x8]
  } }

  // - Processes and thread -

  QList<ulong> getThreadIdsByProcessId(ulong dwOwnerPID);
  QList<ulong> getProcessIdsByParentProcessId(ulong dwOwnerPID);

  bool createProcessWithExecutablePath(const QString &path);

  bool isProcessActiveWithId(ulong dwProcessId);

  QList<ulong> getProcessIdsByName(const QString &processName);
  ulong getProcessIdByName(const QString &processName);

  struct ProcessInfo {
    ulong id;  ///< default is 0
    QString name; ///< name of the executable by default, unless dynamically created

    ProcessInfo() : id(0) { }
  };
  QList<ProcessInfo> getProcessesInfo();

  QString getProcessPathById(ulong dwProcessId);

  struct ModuleInfo {
    ulong moduleId;   ///< default is 0
    ulong processId;  ///< default is 0
    QString moduleName; ///< name of the executable by default, unless dynamically created
    QString modulePath; ///< full path

    ModuleInfo() : moduleId(0), processId(0) { }
  };
  QList<ModuleInfo> getModulesInfo();

  void killCurrentProcess();

  // - Windows -

  ///  Set focus using a native way. If failed, return false but still do it in Qt way.
  bool setFocus(QWidget *w);
  bool setFocus(WId hwnd); // Same as windows version.

  void repaintWindow(WId hwnd);

  WId getChildWindow(WId hwnd);
  QList<WId> getChildWindows(WId hwnd);

  WId getWindowAtPos(const QPoint &globalPos);
  WId getChildWindowAtPos(const QPoint &globalPos, WId parent);

  ulong getWindowThreadId(WId hwnd);
  ulong getWindowProcessId(WId hwnd);

  QRect getWindowRect(WId hwnd);

  bool isValidWindow(WId hwnd);

  bool setTopWindow(WId hwnd);
  WId getTopWindow(WId hwnd = 0); ///< return top window on the desktop if hwnd is null

  WId getPreviousWindow(WId hwnd); ///< below accordint to Z-order
  WId getNextWindow(WId hwnd);     ///< above accordint to Z-order

  bool isWindowAboveWindow(WId parent, WId below);
  bool isWindowBelowWindow(WId parent, WId above);

  // - Mouse and keyboard -
  void sendMouseMove(const QPoint& globalPos, bool relative = false);
  void sendMouseClick(const QPoint& globalPos, Qt::MouseButton button, bool relative = false);

  QPoint getMousePos(); ///< global pos

  int getDoubleClickInterval(); // in msec

  bool isKeyToggled(int vk);
  bool isKeyPressed(int vk);

  bool isKeyCapslockToggled();
  bool isKeyShiftPressed();
  bool isKeyControlPressed();
  bool isKeyWinPressed();

  // - Environments -
  QString getWinDirPath();
  QString getAppDataPath();

  QString getDesktopPath();

  bool isWindowsVistaOrLater();
  bool isWindowsXpOrLater();

  // - Files -

  ///  Create a lnk file at \param lnkPath pointed to \param targetPath.
  bool createLink(const QString &lnkPath, const QString &targetPath, const QString &description = QString());

  ///  Prompt using hwnd if target to return is not valid.
  QString resolveLink(const QString &lnkPath, WId winId = 0);

  // - Devices -

  ///  Consistent with GetDriveType, http://msdn.microsoft.com/en-us/library/windows/desktop/aa364939(v=vs.85).aspx
  enum DriveType {
    UnknownDrive = 0,   // DRIVE_UNKNOWN = 0    The drive type cannot be determined.
    NoRootDirDrive,     // DRIVE_NO_ROOT_DIR = 1  The root path is invalid; for example, there is no volume mounted at the specified path.
    RemovableDrive,     // DRIVE_REMOVABLE = 2  The drive has removable media; for example, a floppy drive, thumb drive, or flash card reader.
    FixedDrive,         // DRIVE_FIXED = 3      The drive has fixed media; for example, a hard disk drive or flash drive.
    RemoteDrive,        // DRIVE_REMOTE = 4     The drive is a remote (network) drive.
    CdRomDrive,         // DRIVE_CDROM = 5      The drive is a CD-ROM drive.
    RamDiskDrive,       // DRIVE_RAMDISK = 6    The drive is a RAM
    DriveTypeCount
  };

// Drive string would look like: "C:\", which takes 3 characters and ends with ":\"
  QStringList getLogicalDrives();
  DriveType getDriveType(const QString &driveLetter);
  QStringList getLogicalDrivesWithType(DriveType type);

#if 0
  ///  \param vol variate in [0, 1]. Return if succeedded.
  bool setWaveVolume(qreal vol);

  ///  Return [0, 1] if succeeded, or-1 if failed.
  qreal getWaveVolume();
#endif // 0

  QString guessDeviceFileName(const QString &hint);
  bool isValidDeviceFileName(const QString &fileName);

  // - Run -

  bool run(const QString &cmd, bool visible = true);

  bool halt();
  bool reboot();
  bool hibernate();
  bool logoff();

  // - String -

  const char *toUtf8(const wchar_t *str);

} // namespace QtWin

// EOF
