// mrlresolvermanager.cc
// 1/25/2012

#include "module/mrlresolver/mrlresolvermanager.h"
#include "module/mrlresolver/mrlresolver.h"
#include "module/mrlresolver/googlevideomrlresolver.h"
#include "module/mrlresolver/luamrlresolver.h"
#include "module/mrlresolver/youkumrlresolver.h"
#include "module/mrlresolver/youtubemrlresolver.h"

//#define DEBUG "mrlresolvermanager"
#include "module/debug/debug.h"

// - Construction -

//QNetworkAccessManager*
//MrlResolverManager::makeNetworkAccessManager()
//{
//  QNetworkAccessManager *ret = new QNetworkAccessManager;
//#ifdef ANNOT_PROXY_DOMAIN
//  ret->setCookieJar(new QtExt::NetworkCookieJarWithDomainAlias(".nicovideo.jp", ANNOT_PROXY_DOMAIN, ret));
//#else
//#  warning "nico alias domain is not defined"
//#endif // ANNOT_PROXY_DOMAIN
//  return ret;
//}

void
MrlResolverManager::init()
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

  connect(this, SIGNAL(synchronizedChanged(bool)), SLOT(updateSynchronized(bool)));
}

void
MrlResolverManager::updateSynchronized(bool t)
{
  foreach(MrlResolver *r, resolvers_)
    r->setSynchronized(t);
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

QString
MrlResolverManager::autoCompleteUrl(const QString &url)
{ return url.startsWith("ttp://") ? QString(url).prepend('h') : url; }

// EOF
