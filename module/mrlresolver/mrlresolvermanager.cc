// mrlresolvermanager.cc
// 1/25/2012

#include "mrlresolvermanager.h"
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
    connect(r, SIGNAL(mediaResolved(MediaInfo)), SIGNAL(mediaResolved(MediaInfo))); \
    connect(r, SIGNAL(annotResolved(QString)), SIGNAL(annotResolved(QString))); \
  } resolvers_.append(r);

  ADD(YoutubeMrlResolver)
  ADD(GoogleVideoMrlResolver)
  ADD(YoukuMrlResolver)
  ADD(LuaMrlResolver)
#undef ADD
}

// - Analysis -

int
MrlResolverManager::match(const QString &href) const
{
  for (int i = 0; i < resolvers_.size(); i++)
    if (resolvers_[i]->match(href))
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
MrlResolverManager::resolveAnnot(int id, const QString &href)
{
  if (id < 0 || id > resolvers_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: resolver id out of bounds");
    return;
  }
  resolvers_[id]->resolveAnnot(href);
}

// EOF
