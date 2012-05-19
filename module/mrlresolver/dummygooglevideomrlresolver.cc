// dummygooglevideomrlresolver.cc
// 2/2/2012

#include "module/mrlresolver/dummygooglevideomrlresolver.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QtCore/QUrl>
#include <cstdlib>

//#define DEBUG "dummygooglevideomrlresolver"
#include "module/debug/debug.h"

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
  static const QString errorMessage = tr("failed to resolve URL");
  QUrl url(href);
  if (url.host().compare("video.google.com", Qt::CaseInsensitive) ||
      url.path().compare("/videoplay", Qt::CaseInsensitive)) {
    emit error(errorMessage + ": " + href);
    //return;
  }

  QString docid = url.queryItemValue("docid");
  if (docid.isEmpty()) {
    emit error(errorMessage + ": " + href);
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
