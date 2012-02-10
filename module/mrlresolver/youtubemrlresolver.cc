// youtubemrlresolver.cc
// 1/25/2012

#include "youtubemrlresolver.h"
#include <QtCore>
#include <QtScript>
#include <QtNetwork>
#include <cstdlib>

//#define DEBUG "youtubemrlresolver"
#include "module/debug/debug.h"

// See: jd/plugins/hoster/YouTubeCom.java
// See: jd/plugins/decrypter/TbCm.java

// - Analysis -

bool
YoutubeMrlResolver::matchMedia(const QString &href) const
{
  QRegExp rx("^http://(|www\\.)youtube\\.com/watch?", Qt::CaseInsensitive);
  return href.contains(rx);
}

// Example: http://www.youtube.com/watch?v=-DJqnomZoLk&list=FLc18abM35KhjkqsLzTmOEjA&feature=plcp
void
YoutubeMrlResolver::resolveMedia(const QString &href)
{
  static const QString errorMessage = tr("failed to resolve URL");
  QUrl url(href);
  if (!url.host().endsWith("youtube.com", Qt::CaseInsensitive) ||
      url.path().compare("/watch", Qt::CaseInsensitive)) {
    emit errorReceived(errorMessage + ": " + href);
    //return;
  }

  QString v = url.queryItemValue("v");
  if (v.isEmpty()) {
    emit errorReceived(errorMessage + ": " + href);
    return;
  }

  QString mrl = "http://youtube.com/watch?v=" + v;
  emit messageReceived(tr("resolving media URL ...") + ": " + mrl);
  MediaInfo mi;
  mi.mrls.append(mrl);
  mi.refurl = mrl;
  emit mediaResolved(mi);
}

// EOF
