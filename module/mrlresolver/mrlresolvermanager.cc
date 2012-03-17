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
    connect(r, SIGNAL(error(QString)), SIGNAL(error(QString))); \
    connect(r, SIGNAL(message(QString)), SIGNAL(message(QString))); \
    connect(r, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*))); \
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
MrlResolverManager::resolverForMedia(const QString &href) const
{
  for (int i = 0; i < resolvers_.size(); i++)
    if (resolvers_[i]->matchMedia(href))
      return i;
  return -1;
}

int
MrlResolverManager::resolverForSubtitle(const QString &href) const
{
  for (int i = 0; i < resolvers_.size(); i++)
    if (resolvers_[i]->matchSubtitle(href))
      return i;
  return -1;
}

bool
MrlResolverManager::resolveMedia(int id, const QString &href)
{
  if (id < 0 || id > resolvers_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: resolver id out of bounds");
    return false;
  }
  return resolvers_[id]->resolveMedia(href);
}

bool
MrlResolverManager::resolveSubtitle(int id, const QString &href)
{
  if (id < 0 || id > resolvers_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: resolver id out of bounds");
    return false;
  }
  return resolvers_[id]->resolveSubtitle(href);
}

// EOF
