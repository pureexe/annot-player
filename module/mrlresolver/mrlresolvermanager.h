#ifndef MRLRESOLVERMANAGER_H
#define MRLRESOLVERMANAGER_H

// mrlresolvermanager.h
// 1/25/2011

#include "mediainfo.h"
#include <QObject>
#include <QList>

class MrlResolver;

class MrlResolverManager : public QObject
{
  Q_OBJECT
  typedef MrlResolverManager Self;
  typedef QObject Base;

  QList<MrlResolver*> resolvers_;

public:
  enum Resolver { Youtube = 0, GoogleVideo, Youku, Lua, ResolverCount };

  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit MrlResolverManager(QObject *parent = 0);

signals:
  void errorReceived(QString message);
  void messageReceived(QString message);

  void mediaResolved(MediaInfo mi);
  void annotResolved(QString suburl);

public:
  int match(const QString &href) const;

  void resolveMedia(int id, const QString &href);

  bool resolveMedia(const QString &href)
  {
    int r = match(href);
    if (r < 0)
      return false;
    resolveMedia(r, href);
    return true;
  }

  void resolveAnnot(int id, const QString &href);
  bool resolveAnnot(const QString &href)
  {
    int r = match(href);
    if (r != Lua)
      return false;
    resolveAnnot(r, href);
    return true;
  }
};

#endif // MRLRESOLVERMANAGER_H
