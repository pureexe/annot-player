// dummyyoutubemrlresolver.cc
// 2/2/2012

#include "lib/mrlresolver/dummyyoutubemrlresolver.h"
#include "lib/mrlanalysis/mrlanalysis.h"
#include <QtCore/QUrl>
#include <cstdlib>

//#define DEBUG "dummyoutubemrlresolver"
#include "qtx/qxdebug.h"

// See: lua/playlist/googlevideo.lua

// - Analysis -

bool
DummyYoutubeMrlResolver::matchMedia(const QString &href) const
{
  QString pattern("http://" MA_EIGEN_YOUTUBE);
  return href.startsWith(pattern, Qt::CaseInsensitive);
}

bool
DummyYoutubeMrlResolver::resolveMedia(const QString &href)
{
  QUrl url(href);
  if (url.host().compare("www.youtube.com", Qt::CaseInsensitive) ||
      url.path().compare("/watch", Qt::CaseInsensitive)) {
    emit errorMessage(tr("failed to resolve URL") + ": " + href);
    //return;
  }

  QString v = url.queryItemValue("v");
  if (v.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL") + ": " + href);
    return false;
  }

  QString mrl = "http://www.youtube.com/watch?v=" + v;
  emit message(tr("resolving media URL ...") + ": " + mrl);
  MediaInfo mi;
  mi.mrls.append(MrlInfo(mrl));
  mi.refurl = mrl;
  emit mediaResolved(mi);
  return true;
}

// EOF
