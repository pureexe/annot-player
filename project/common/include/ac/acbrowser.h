#ifndef ACBROWSER_H
#define ACBROWSER_H

// acbrowser.h
// 4/9/2012

#include "ac/acipc.h"

class AcBrowserController : public QObject
{
  Q_OBJECT
  typedef AcBrowserController Self;
  typedef QObject Base;

  // - Construction -
public:
  static Self *globalController() { static Self g; return &g; }
  explicit AcBrowserController(QObject *parent = 0, AcIpcController::Role role = AcIpcController::NoRole);

//signals:
//  void urlsRequested(const QString &urls);

  // - Queries -
public:
  bool isRunning() const;

  // - Actions
public slots:
  void stop() { }
  void open();
  void openUrl(const QString &url);
  void openUrls(const QStringList &urls);
};

#endif // ACBROWSER_H
