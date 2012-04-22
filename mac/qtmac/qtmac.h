#ifndef QTMAC_H
#define QTMAC_H

// qtmac.h
// 11/11/2011

#include <QtCore/QString>
#include <QtCore/QStringList>

#define XA_RESOURCEFORK      "com.apple.ResourceFork"

#define XA_FINDERINFO        "com.apple.FinderInfo"
#define XA_FINDERINFO_NULL              "0000000000000000000000000000000000000000000000000000000000000000"
#define XA_FINDERINFO_CUSTOMICON        "0000000000000000040000000000000000000000000000000000000000000000"
#define XA_FINDERINFO_ICONMACS          "69636F6E4D414353400000000000000000000000000000000000000000000000"
#define XAB_FINDERINFO_ICONMACS         "iconMACS@"

namespace QtMac {

  // - Environment -

  QString homeLibraryPath();
  QString homeMusicPath();
  QString homeMoviesPath();
  QString homeDownloadsPath();
  QString homeDocumentsPath();
  QString homeCachesPath();
  QString homeLogsPath();
  QString homeApplicationSupportPath();

  // - Devices -

#ifdef WITH_IOKIT
  QStringList getCDMediaPaths();
#endif // WITH_IOKIT

  // - Script -

  ///  Return error number.
  int run(const QString &bin, const QStringList &args = QStringList(), int timeoutMsecs = -1);
  bool open(const QString &app, const QStringList &args = QStringList());

  bool halt();
  bool reboot();
  bool sleep();

  // See: man SetFile
  enum FileAttribute {
    FA_CustomIcon = 1 << 3 // SetFile -a C
  };

  enum Mode { BinaryMode = 0, TextMode };

  bool setFileAttributes(const QString &fileName, uint flags); ///< SetFile -a flags fileName
  bool removeFileAttributes(const QString &fileName); ///< xattr -d com.apple.FinderInfo fileName
  bool setFileExtendedAttribute(const QString &fileName, const char *key, const QByteArray &value, Mode mode = BinaryMode);

} // namespace QtMac

#endif // QTMAC_H
