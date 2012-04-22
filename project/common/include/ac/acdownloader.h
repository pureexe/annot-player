#ifndef _AC_ACDOWNLOADER_H
#define _AC_ACDOWNLOADER_H

// acdownloader.h
// 4/9/2012

#include "ac/acipc.h"

class AcDownloaderServer : public AcIpcController
{
  Q_OBJECT
  typedef AcDownloaderServer Self;
  typedef AcIpcController Base;

  static Self *global_;

  // - Construction -
public:
  explicit AcDownloaderServer(QObject *parent = 0);
  ~AcDownloaderServer() { stop(); }

public slots:
  void start();
  void stop();
};

class AcDownloader : public QObject
{
  Q_OBJECT
  typedef AcDownloader Self;
  typedef QObject Base;
  typedef AcIpcController Delegate;

  Delegate *delegate_;

public:
  explicit AcDownloader(QObject *parent = 0);

signals:
  void arguments(const QStringList &args);

public:
  bool isRunning() const;

public slots:
  void open();
  void openArguments(const QStringList &args);
  void openUrls(const QStringList &urls) { openArguments(urls); }
  void openUrl(const QString &url) { openUrls(QStringList(url)); }
};

#endif // _AC_ACDOWNLOADER_H
