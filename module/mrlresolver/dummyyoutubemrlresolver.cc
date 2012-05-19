// dummyyoutubemrlresolver.cc
// 2/2/2012

#include "module/mrlresolver/dummyyoutubemrlresolver.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QtCore/QUrl>
#include <cstdlib>

//#define DEBUG "dummyoutubemrlresolver"
#include "module/debug/debug.h"

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
  static const QString errorMessage = tr("failed to resolve URL");
  QUrl url(href);
  if (url.host().compare("www.youtube.com", Qt::CaseInsensitive) ||
      url.path().compare("/watch", Qt::CaseInsensitive)) {
    emit error(errorMessage + ": " + href);
    //return;
  }

  QString v = url.queryItemValue("v");
  if (v.isEmpty()) {
    emit error(errorMessage + ": " + href);
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
