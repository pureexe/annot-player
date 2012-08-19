#pragma once

// qtwin.h
// 7/21/2011

#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QWidget>

#define QTWIN_INVALID_POS QPoint(-32000, -32000)

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

  // - File system -

  // See: http://msdn.microsoft.com/en-us/library/windows/desktop/aa365535(v=vs.85).aspx
  enum FileAttribute {
    NoAttribute = 0x0,
    ArchiveAttribute = 0x20,    // 32, A file or directory that is an archive file or directory. Applications typically use this attribute to mark files for backup or removal.
    HiddenAttribute = 0x2,      // 2, The file or directory is hidden. It is not included in an ordinary directory listing.
    NormalAttribute = 0x80,     // 128, A file that does not have other attributes set. This attribute is valid only when used alone.
    OfflineAttribute = 0x1000,  // 4096, The data of a file is not available immediately. This attribute indicates that the file data is physically moved to offline storage. This attribute is used by Remote Storage, which is the hierarchical storage management software. Applications should not arbitrarily change this attribute.
    ReadOnlyAttribute = 0x1,    // 1,  A file that is read-only. Applications can read the file, but cannot write to it or delete it. This attribute is not honored on directories. For more information, see "You cannot view or change the Read-only or the System attributes of folders in Windows Server 2003, in Windows XP, or in Windows Vista".
    SystemAttribute = 0x4,      // 4,  A file or directory that the operating system uses a part of, or uses exclusively.
    TemporaryAttribute = 0x100  // 256, A file that is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because typically, an application deletes a temporary file after the handle is closed. In that scenario, the system can entirely avoid writing the data. Otherwise, the data is written after the handle is closed.
  };

  bool setFileAttributes(const QString &fileName, uint attributes);

  // - Window -

  ///  Set focus using a native way. If failed, return false but still do it in Qt way.
  bool setFocus(QWidget *w);
  bool setFocus(WId hwnd); // Same as windows version.

  void repaintWindow(WId hwnd);

  WId getChildWindow(WId hwnd);
  QList<WId> getChildWindows(WId hwnd);

  WId getWindowAtPos(const QPoint &globalPos);
  WId getChildWindowAtPos(const QPoint &globalPos, WId parent);

  ///  Reply from WM_GETTEXT, in most case the window's title
  QString getWindowText(WId hwnd, size_t size = 0);

  bool windowHasText(WId hwnd);

  ulong getWindowThreadId(WId hwnd);
  ulong getWindowProcessId(WId hwnd);

  WId getWindowWithThreadId(ulong threadId);
  WId getWindowWithProcessId(ulong processId);

  WId getVisibleWindowWithThreadId(ulong threadId);
  WId getVisibleWindowWithProcessId(ulong processId);

  WId getGoodWindowWithThreadId(ulong threadId);
  WId getGoodWindowWithProcessId(ulong processId);

  //QList<WId> getWindowsWithThreadId(ulong threadId);
  //QList<WId> getWindowsWithProcessId(ulong processId);

  ///  Return QTWIN_INVALID_POS when miniized
  QRect getWindowRect(WId hwnd);
  bool windowHasRect(WId hwnd);

  bool isWindowValid(WId hwnd);

  bool isWindowVisible(WId hwnd);

  bool isWindowMinimized(WId hwnd);

  ///  valid, visible, has non-empty title and non-empty rect
  bool isGoodWindow(WId hwnd);

  bool setTopWindow(WId hwnd);
  WId getTopWindow(WId hwnd = nullptr); ///< return top window on the desktop if hwnd is null

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
  bool isKeyAltPressed();
  bool isKeyControlPressed();
  bool isKeyWinPressed();

  bool isMouseLeftButtonPressed();
  bool isMouseMiddleButtonPressed();
  bool isMouseRightButtonPressed();

  // - Environments -
  bool isWindowsVistaOrLater();
  bool isWindowsXpOrLater();

  QString getPath();
  QString getWinDirPath();
  QString getSystemRoot();
  QString getProgramFilesPath();
  QString getAppDataPath();

  QString getFontsPath();
  QString getMusicPath();
  QString getVideoPath();
  QString getDocumentsPath();
  QString getDownloadsPath();

  bool setPath(const QString &value);
  bool prependPath(const QString &path);
  bool appendPath(const QString &path);

  // - Files -

  ///  Create a lnk file at \param lnkPath pointed to \param targetPath.
  bool createLink(const QString &lnkPath, const QString &targetPath, const QString &description = QString());

  ///  Prompt using hwnd if target to return is not valid.
  QString resolveLink(const QString &lnkPath, WId hwnd = nullptr);

  ///  Move to Recycle Bin instead of delete. Complete path required.
  bool trashFile(const QString &fileFullPath, bool confirm = false, WId hwnd = nullptr);

  bool isFileExists(const QString &fileName);
  bool deleteFile(const QString &fileName);

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

  char getFileDriveLetter(const QString &fileName);

  // - Run -

  bool run(const QString &cmd, bool visible = true);

  bool halt();
  bool reboot();
  bool hibernate();
  bool logoff();

  // - Shell -

  void addRecentDocument(const QString &fileName);

  // - String -

  const char *toUtf8(const wchar_t *str);

} // namespace QtWin

// EOF
