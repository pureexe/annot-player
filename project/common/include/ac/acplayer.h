#ifndef ACPLAYER_H
#define ACPLAYER_H

// acplayer.h
// 4/9/2012

#include "ac/acipc.h"

class AcPlayerController : public AcIpcController
{
  Q_OBJECT
  typedef AcPlayerController Self;
  typedef AcIpcController Base;

  // - Construction -
public:
  static Self *globalController() { static Self g; return &g; }
  explicit AcPlayerController(QObject *parent = 0, Role role = NoRole);

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

#endif // ACPLAYER_H
