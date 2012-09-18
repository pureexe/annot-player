// dummygooglevideomrlresolver.cc
// 2/2/2012

#include "lib/mrlresolver/dummygooglevideomrlresolver.h"
#include "lib/mrlanalysis/mrlanalysis.h"
#include <QtCore/QUrl>
#include <cstdlib>

//#define DEBUG "dummygooglevideomrlresolver"
#include "qtx/qxdebug.h"

// See: lua/playlist/googlevideo.lua

// - Analysis -

bool
DummyGoogleVideoMrlResolver::matchMedia(const QString &href) const
{
  QString pattern("http://" MA_EIGEN_GOOGLEVIDEO);
  return href.startsWith(pattern, Qt::CaseInsensitive);
}

// Example: http://video.google.com/videoplay?docid=-8070240344560020977
bool
DummyGoogleVideoMrlResolver::resolveMedia(const QString &href)
{
  QUrl url(href);
  if (url.host().compare("video.google.com", Qt::CaseInsensitive) ||
      url.path().compare("/videoplay", Qt::CaseInsensitive)) {
    emit errorMessage(tr("failed to resolve URL") + ": " + href);
    //return;
  }

  QString docid = url.queryItemValue("docid");
  if (docid.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL") + ": " + href);
    return false;
  }

  QString mrl = "http://video.google.com/videoplay?docid=" + docid;
  emit message(tr("resolving media URL ...") + ": " + mrl);
  MediaInfo mi;
  mi.mrls.append(MrlInfo(mrl));
  mi.refurl = mrl;
  emit mediaResolved(mi);
  return true;
}

// EOF
