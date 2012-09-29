#pragma once

// qtwin.h
// 7/21/2011 jichi

#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QStringList>
#include <QtGui/qwindowdefs.h> // for WId

QT_FORWARD_DECLARE_CLASS(QWidget)

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

  // - Main module -

  ///  Return main module path as LPWSTR
  inline const wchar_t *getModulePath()
  {
    // fs:[0x30]: PEB, see: ntdll.h, see also: http://en.wikipedia.org/wiki/Win32_Thread_Information_Block
    // PEB+0xC: Ldr (PPEB_LDR_DATA)
    __asm {
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

  ///  \var environ is a list of assignments, such as "var=value"
  bool createProcess(const QString &path, const QStringList &environ = QStringList());

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
  ulong getFileAttributes(const QString &fileName);

  inline bool isFileHidden(const QString &fileName)
  { return HiddenAttribute & getFileAttributes(fileName); }

  inline bool isFileReadOnly(const QString &fileName)
  { return ReadOnlyAttribute & getFileAttributes(fileName); }

  inline bool removeFileAttributes(const QString &fileName)
  { return setFileAttributes(fileName, NormalAttribute); }

  // - Window -

#ifdef QT_WIDGETS_LIB
  ///  Set focus using a native way. If failed, return false but still do it in Qt way.
  bool setFocus(QWidget *w);
#endif // QT_WIDGETS_LIB
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

  ///  Return top window on the desktop if hwnd is null
  WId getTopWindow(WId hwnd = 0);
  bool setTopWindow(WId hwnd);

  ///  Below accordint to Z-order
  WId getPreviousWindow(WId hwnd);
  ///  Above accordint to Z-order
  WId getNextWindow(WId hwnd);

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

  // - Cursors -

  // See: http://msdn.microsoft.com/en-us/library/windows/desktop/ms648071%28v=vs.85%29.aspx
  enum CursorShape { // Name consistent with Qt::CursorShape, value constent with OCR_

    NullCursor = 0,
    BusyCursor = 32650,         // IDC_APPSTARTING MAKEINTRESOURCE(32650)   Standard arrow and small hourglass cursor.
    ArrrowCursor = 32512,       // IDC_ARROW MAKEINTRESOURCE(32512)     Standard arrow cursor.
    CrossCursor = 32515,        // IDC_CROSS MAKEINTRESOURCE(32515)     Crosshair cursor.
    PointingHandCursor = 32649, // IDC_HAND MAKEINTRESOURCE(32649)      Hand cursor.
    WhatsThisCursor = 32651,    // IDC_HELP MAKEINTRESOURCE(32651)      Arrow and question mark cursor.
    IBeamCursor = 32513,        // IDC_IBEAM MAKEINTRESOURCE(32513)     I-beam cursor.
    ForbiddenCursor = 32648,    // IDC_NO MAKEINTRESOURCE(32648)        Slashed circle cursor.
    SizeAllCursor = 32646,      // IDC_SIZEALL MAKEINTRESOURCE(32646)   Four-pointed arrow cursor pointing north, south, east, and west.
    SizeBDiagCursor = 32643,    // IDC_SIZENESW MAKEINTRESOURCE(32643)  Double-pointed arrow cursor pointing northeast and southwest.
    SizeVerCursor = 32645,      // IDC_SIZENS MAKEINTRESOURCE(32645)    Double-pointed arrow cursor pointing north and south.
    SizeFDiagCursor = 32642,    // IDC_SIZENWSE MAKEINTRESOURCE(32642)  Double-pointed arrow cursor pointing northwest and southeast.
    SizeHorCursor = 32644,      // IDC_SIZEWE MAKEINTRESOURCE(32644)    Double-pointed arrow cursor pointing west and east.
    UpArrow = 32516,            // IDC_UPARROW MAKEINTRESOURCE(32516)   Vertical arrow cursor.
    WaitArrow = 32514           // IDC_WAIT MAKEINTRESOURCE(32514)      Hourglass cursor.
    // IDI_APPLICATION MAKEINTRESOURCE(32512)  Application icon.
    // IDI_ASTERISK MAKEINTRESOURCE(32516)     Asterisk icon.
    // IDI_EXCLAMATION MAKEINTRESOURCE(32515)  Exclamation point icon.
    // IDI_HAND MAKEINTRESOURCE(32513)         Stop sign icon.
    // IDI_QUESTION MAKEINTRESOURCE(32514)     Question-mark icon.
    // IDI_WINLOGO MAKEINTRESOURCE(32517)      Application icon.  Windows 2000:  Windows logo icon.
  };
  typedef void *CursorHandle;

  ///  Return previous cursor.
  CursorHandle setCursor(CursorHandle hcur);
  CursorHandle getCursor();
  CursorHandle loadCursorFromFile(const QString &fileName);
  CursorHandle loadCursorFromFile(const wchar_t *fileName);
  CursorHandle copyCursor(CursorHandle hcur);
  bool destroyCursor(CursorHandle hcur);
  bool setSystemCursor(CursorHandle hcur, CursorShape shape);

  // - Environments -
  bool isWindowsVistaOrLater();
  bool isWindowsXpOrLater();

  QString getPath();
  QString getWinDirPath();
  QString getSystemRoot();
  QString getProgramFilesPath();
  QString getAppDataPath();
  QString getLocalAppDataPath();

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
  QString resolveLink(const QString &lnkPath, WId hwnd = 0);

  ///  Move to Recycle Bin instead of delete. Complete path required.
  bool trashFile(const QString &fileFullPath, bool confirm = false, WId hwnd = 0);

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
