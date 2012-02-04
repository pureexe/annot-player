// googlevideomrlresolver.cc
// 2/2/2012

#include "googlevideomrlresolver.h"
#include <QtCore>
#include <QtScript>
#include <QtNetwork>
#include <cstdlib>

//#define DEBUG "googlevideomrlresolver"
#include "module/debug/debug.h"

// See: lua/playlist/googlevideo.lua

// - Analysis -

bool
GoogleVideoMrlResolver::match(const QString &href) const
{
  QString pattern("http://video.google.com/videoplay?docid=");
  return href.startsWith(pattern, Qt::CaseInsensitive);
}

// Example: http://video.google.com/videoplay?docid=-8070240344560020977
void
GoogleVideoMrlResolver::resolveMedia(const QString &href)
{
  static const QString errorMessage = tr("failed to resolve URL");
  QUrl url(href);
  if (url.host().compare("video.google.com", Qt::CaseInsensitive) ||
      url.path().compare("/videoplay", Qt::CaseInsensitive)) {
    emit errorReceived(errorMessage + ": " + href);
    //return;
  }

  QString docid = url.queryItemValue("docid");
  if (docid.isEmpty()) {
    emit errorReceived(errorMessage + ": " + href);
    return;
  }

  QString mrl = "http://video.google.com/videoplay?docid=" + docid;
  emit messageReceived(tr("resolving media URL ...") + ": " + mrl);
  MediaInfo mi;
  mi.mrls.append(mrl);
  mi.refurl = mrl;
  emit mediaResolved(mi);
}

// EOF
