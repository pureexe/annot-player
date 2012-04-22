#ifndef _AC_ACPLAYER_H
#define _AC_ACPLAYER_H

// acplayer.h
// 4/9/2012

#include "ac/acipc.h"

class AcPlayerServer : public AcIpcController
{
  Q_OBJECT
  typedef AcPlayerServer Self;
  typedef AcIpcController Base;

  static Self *global_;

  // - Construction -
public:
  explicit AcPlayerServer(QObject *parent = 0);
  ~AcPlayerServer() { stop(); }

public slots:
  void start();
  void stop();
};

class AcPlayer : public QObject
{
  Q_OBJECT
  typedef AcPlayer Self;
  typedef QObject Base;
  typedef AcIpcController Delegate;

  Delegate *delegate_;
public:
  explicit AcPlayer(QObject *parent = 0);

signals:
  void arguments(const QStringList &args);

public:
  bool isRunning() const;

public slots:
  void open();
  void openArguments(const QStringList &args);

  void openUrls(const QStringList &urls) { openArguments(urls); }
  void openUrl(const QString &url) { openUrls(QStringList(url)); }

  void importUrl(const QString &url);
  void importUrls(const QStringList &urls)
  { foreach (const QString &url, urls) importUrl(url); }
};

#endif // _AC_ACPLAYER_H
