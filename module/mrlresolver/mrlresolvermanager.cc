// mrlresolvermanager.cc
// 1/25/2012

#include "mrlresolvermanager.h"
#include "youtubemrlresolver.h"
#include "youkumrlresolver.h"
#include <QtCore>
#include <QtScript>
#include <QtNetwork>
#include <cstdlib>

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
    connect(r, SIGNAL(resolved(QStringList,QString,QString)), SIGNAL(resolved(QStringList,QString,QString))); \
  } resolvers_.append(r);

  ADD(YoukuMrlResolver)
  ADD(YoutubeMrlResolver)
#undef ADD
}

// - Analysis -

int
MrlResolverManager::match(const QString &href) const
{
  for (int i = 0; i <  resolvers_.size(); i++)
    if (resolvers_[i]->match(href))
      return i;
  return -1;
}

void
MrlResolverManager::resolve(int id, const QString &href)
{
  if (id < 0 || id > resolvers_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: resolver id out of bounds");
    return;
  }
  resolvers_[id]->resolve(href);
}

// EOF
