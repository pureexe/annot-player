#ifndef MRLRESOLVER_H
#define MRLRESOLVER_H

// mrlresolver.h
// 1/24/2011

#include "mrlinfo.h"
#include <QObject>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class MrlResolver : public QObject
{
  Q_OBJECT
  typedef MrlResolver Self;
  typedef QObject Base;

public:
  explicit MrlResolver(QObject *parent = 0)
    : Base(parent) { }

signals:
  void errorReceived(QString message);
  void messageReceived(QString message);

  void mediaResolved(MediaInfo mi, QNetworkAccessManager *nam);
  void subtitleResolved(QString suburl);

public:
  virtual bool matchMedia(const QString &href) const { (void)href; return false; }
  virtual bool matchSubtitle(const QString &href) const { (void)href; return false; }
public slots:
  virtual void resolveMedia(const QString &href)  { (void)href; }

  virtual void resolveSubtitle(const QString &href) { (void)href; }
};

#endif // MRLRESOLVER_H
