#ifndef MRLRESOLVER_H
#define MRLRESOLVER_H

// mrlresolver.h
// 1/24/2011

#include "mediainfo.h"
#include <QObject>

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

  void mediaResolved(MediaInfo mi);
  void annotResolved(QString suburl);

public:
  virtual bool match(const QString &href) const = 0;
public slots:
  virtual void resolveMedia(const QString &href) = 0;

  virtual void resolveAnnot(const QString &href) { (void)href; }
};

#endif // MRLRESOLVER_H
