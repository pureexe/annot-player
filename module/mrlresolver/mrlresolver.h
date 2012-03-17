#ifndef MRLRESOLVER_H
#define MRLRESOLVER_H

// mrlresolver.h
// 1/24/2011

#include "mrlinfo.h"
#include <QObject>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class MrlResolver : public QObject
{
  Q_OBJECT
  typedef MrlResolver Self;
  typedef QObject Base;

public:
  explicit MrlResolver(QObject *parent = 0)
    : Base(parent) { }

signals:
  void error(QString msg);
  void message(QString msg);

  void mediaResolved(MediaInfo mi, QNetworkCookieJar *cookieJar);
  void subtitleResolved(QString suburl);

public:
  virtual bool matchMedia(const QString &href) const { (void)href; return false; }
  virtual bool matchSubtitle(const QString &href) const { (void)href; return false; }
public slots:
  virtual bool resolveMedia(const QString &href)  { (void)href; return false; }

  virtual bool resolveSubtitle(const QString &href) { (void)href; return false; }
};

#endif // MRLRESOLVER_H
