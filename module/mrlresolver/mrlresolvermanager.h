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

  // - Types -
public:
  struct Account {
    QString username, password;

    Account(const QString &name, const QString &pass)
      : username(name), password(pass) { }
    void clear() { username.clear(); password.clear(); }
  };

  // - Construction -
public:
  enum Resolver { Youtube = 0, GoogleVideo, Youku, Lua, ResolverCount };

  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit MrlResolverManager(QObject *parent = 0);

  // - Signals -
signals:
  void errorReceived(QString message);
  void messageReceived(QString message);

  void mediaResolved(MediaInfo mi);
  void subtitleResolved(QString suburl);

  // - Analysis -
public:
  int matchMedia(const QString &href) const;
  int matchSubtitle(const QString &href) const;

  void resolveMedia(int id, const QString &href);

  bool resolveMedia(const QString &href)
  {
    int r = matchMedia(href);
    if (r < 0)
      return false;
    resolveMedia(r, href);
    return true;
  }

  void resolveSubtitle(int id, const QString &href);
  bool resolveSubtitle(const QString &href)
  {
    int r = matchSubtitle(href);
    if (r != Lua)
      return false;
    resolveSubtitle(r, href);
    return true;
  }

public slots:
  void setNicovideoAccount(const QString &username = QString(),
                           const QString &password = QString());
  void setBilibiliAccount(const QString &username = QString(),
                          const QString &password = QString());
public:
  Account nicovideoAccount() const;
  Account bilibiliAccount() const;
};

#endif // MRLRESOLVERMANAGER_H
