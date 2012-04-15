#ifndef ACDOWNLOADER_H
#define ACDOWNLOADER_H

// acdownloader.h
// 4/9/2012

#include "ac/acipc.h"

class AcDownloaderController : public AcIpcController
{
  Q_OBJECT
  typedef AcDownloaderController Self;
  typedef AcIpcController Base;

  // - Construction -
public:
  static Self *globalController() { static Self g; return &g; }
  explicit AcDownloaderController(QObject *parent = 0, Role role = NoRole);

signals:
  void urlsRequested(const QString &urls);

  // - Queries -
public:
  bool isRunning() const;

  // - Actions
public slots:
  void open();
  void openUrl(const QString &url);
  void openUrls(const QStringList &urls);
};

#endif // ACDOWNLOADER_H
