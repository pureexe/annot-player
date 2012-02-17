// mrlresolvermanager.cc
// 1/25/2012

#include "mrlresolvermanager.h"
#include "mrlresolver.h"
#include "youtubemrlresolver.h"
#include "googlevideomrlresolver.h"
#include "youkumrlresolver.h"
#include "luamrlresolver.h"
#include <QtCore>

//#define DEBUG "mrlresolvermanager"
#include "module/debug/debug.h"

// - Construction -

MrlResolverManager::MrlResolverManager(QObject *parent)
  : Base(parent)
{
  MrlResolver *r;
#define ADD(_resolver) \
  r = new _resolver(this); { \
    connect(r, SIGNAL(errorReceived(QString)), SIGNAL(errorReceived(QString))); \
    connect(r, SIGNAL(messageReceived(QString)), SIGNAL(messageReceived(QString))); \
    connect(r, SIGNAL(mediaResolved(MediaInfo,QNetworkAccessManager*)), SIGNAL(mediaResolved(MediaInfo,QNetworkAccessManager*))); \
    connect(r, SIGNAL(subtitleResolved(QString)), SIGNAL(subtitleResolved(QString))); \
  } resolvers_.append(r);

  ADD(YoutubeMrlResolver)
  ADD(GoogleVideoMrlResolver)
  //ADD(YoukuMrlResolver)
  ADD(LuaMrlResolver)
#undef ADD
}

// - Analysis -

int
MrlResolverManager::matchMedia(const QString &href) const
{
  for (int i = 0; i < resolvers_.size(); i++)
    if (resolvers_[i]->matchMedia(href))
      return i;
  return -1;
}

int
MrlResolverManager::matchSubtitle(const QString &href) const
{
  for (int i = 0; i < resolvers_.size(); i++)
    if (resolvers_[i]->matchSubtitle(href))
      return i;
  return -1;
}

void
MrlResolverManager::resolveMedia(int id, const QString &href)
{
  if (id < 0 || id > resolvers_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: resolver id out of bounds");
    return;
  }
  resolvers_[id]->resolveMedia(href);
}

void
MrlResolverManager::resolveSubtitle(int id, const QString &href)
{
  if (id < 0 || id > resolvers_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: resolver id out of bounds");
    return;
  }
  resolvers_[id]->resolveSubtitle(href);
}

// - Account -

void
MrlResolverManager::setNicovideoAccount(const QString &userName, const QString &password)
{
  LuaMrlResolver *r = dynamic_cast<LuaMrlResolver*>(resolvers_[Lua]);
  Q_ASSERT(r);
  if (userName.isEmpty() || password.isEmpty())
    r->clearNicovideoAccount();
  else
    r->setNicovideoAccount(userName, password);
}

void
MrlResolverManager::setBilibiliAccount(const QString &userName, const QString &password)
{
  LuaMrlResolver *r = dynamic_cast<LuaMrlResolver*>(resolvers_[Lua]);
  Q_ASSERT(r);
  if (userName.isEmpty() || password.isEmpty())
    r->clearBilibiliAccount();
  else
    r->setBilibiliAccount(userName, password);
}

MrlResolverManager::Account
MrlResolverManager::nicovideoAccount() const
{
  LuaMrlResolver *r = dynamic_cast<LuaMrlResolver*>(resolvers_[Lua]);
  Q_ASSERT(r);
  return Account(r->nicovideoUsername(), r->nicovideoPassword());
}

MrlResolverManager::Account
MrlResolverManager::bilibiliAccount() const
{
  LuaMrlResolver *r = dynamic_cast<LuaMrlResolver*>(resolvers_[Lua]);
  Q_ASSERT(r);
  return Account(r->bilibiliUsername(), r->bilibiliPassword());
}

// EOF
