// youtubemrlresolver.cc
// 1/25/2012
//
// JDownloader:
// svn://svn.jdownloader.org/jdownloader/trunk/src/jd
// svn://svn.jdownloader.org/jdownloader/browser
//
// See: jd/plugins/hoster/Youtube.java
// See: jd/plugins/decrypter/TbCm.java
// See: AcDown/Downloader/YouTube/YouTubeDownloader.cs
// See: AcDown/Downloader/YouTube/YouTubePlugin.cs

#include "module/mrlresolver/youtubemrlresolver.h"
#include "module/qtext/htmltag.h"
//#include "module/qtext/network.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <cstdlib>

#define DEBUG "youtubemrlresolver"
#include "module/debug/debug.h"

// - Construction -

void
YoutubeMrlResolver::init()
{
  nam_ = new QNetworkAccessManager(this);
  connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(resolveMedia(QNetworkReply*)));
}

// - Analysis -

bool
YoutubeMrlResolver::matchMedia(const QString &href) const
{
  QRegExp rx("^http://(|www\\.)youtube\\.com/watch?", Qt::CaseInsensitive);
  return href.contains(rx);
}

// Example: http://www.youtube.com/watch?v=-DJqnomZoLk&list=FLc18abM35KhjkqsLzTmOEjA&feature=plcp
bool
YoutubeMrlResolver::resolveMedia(const QString &href)
{
  DOUT("enter: href =" << href);
  QUrl url(href);
  if (url.path().compare("/watch", Qt::CaseInsensitive) ||
      (url.host().compare("youtube.com", Qt::CaseInsensitive) &&
       url.host().compare("www.youtube.com"))
      ) {
    emit errorMessage(tr("failed to resolve URL") + ": " + href);
    DOUT("exit: mimatched host or path");
    return false;
  }

  QString v = url.queryItemValue("v");
  if (v.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL") + ": " + href);
    DOUT("exit: mimatched query value");
    return false;
  }

  url = "http://www.youtube.com/watch?v=" + v;
  DOUT("url =" << url);
  nam_->get(QNetworkRequest(url));
  //QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(url));
  //connect(new QtExt::NetworkReplyFinished(reply), SIGNAL(finished(QNetworkReply*)), SLOT(resolveMedia(QNetworkReply*)));
  DOUT("exit: ret = true");
  return true;
}

// - Download -

void
YoutubeMrlResolver::resolveMedia(QNetworkReply *reply)
{
  DOUT("enter");

  Q_ASSERT(reply);
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    //emit errorMessage(reply->errorString());
    emit errorMessage(tr("network error, failed to resolve media URL") + ": " + reply->url().toString());
    DOUT("exit: error =" << reply->error());
    return;
  }

  QString data = reply->readAll();
  if (data.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL")+ ": " + reply->url().toString());
    DOUT("exit: empty data");
    return;
  }

  QString mrl = reply->url().toString();
  QString title;
  QUrl url;
  QList<QUrl> urls;

  // Escape percent encoding.
  QRegExp
  rx = QRegExp("<title>(.*)</title>");
  qint64 offset; // use offset to improve performance by sequential search
  if ((offset=rx.indexIn(data)) < 0)
    offset = 0;
  else {
    Q_ASSERT(rx.captureCount() == 1);
    title = formatTitle(rx.cap(1).simplified());
  }

  rx = QRegExp("url_encoded_fmt_stream_map=([^;]*);");
  if ((offset=rx.indexIn(data, offset)) < 0)
    offset = 0;
  else {
    Q_ASSERT(rx.captureCount() == 1);
    QString map = rx.cap(1);
    map = QUrl::fromPercentEncoding(map.toAscii());
    foreach (QString url, map.split(",", QString::SkipEmptyParts)) {
      Q_ASSERT(url.startsWith("url="));
      url.remove(QRegExp("^url="));
      url = QUrl::fromPercentEncoding(url.toAscii());
      urls.append(url);
    }
  }

  // URLs are in quality-desc order, such as: <quality, type>
  // - "large" "video/webm;+codecs="vp8.0,+vorbis""
  // - "large" "video/x-flv"
  // - "medium" "video/webm;+codecs="vp8.0,+vorbis""
  // - "medium" "video/x-flv"
  // - "medium" "video/mp4;+codecs="avc1.42001E,+mp4a.40.2""
  // - "small" "video/x-flv"
  //
  // Always prefer WebM (i.e. MKV) and the one with the best quality
  if (!urls.isEmpty())
    url = urls.first();

  if (url.isEmpty())  {
    emit errorMessage(tr("failed to resolve URL") + ": " + reply->url().toString());
    DOUT("exit: empty data");
    return;
  }

  QString videoUrl = decodeYoutubeUrl(url);
  DOUT("url =" << videoUrl);

  MediaInfo mi;
  mi.title = title;
  mi.refurl = mrl;
  mi.mrls.append(MrlInfo(videoUrl));
  emit mediaResolved(mi);
  DOUT("exit");
}

// - Format -

QString
YoutubeMrlResolver::formatTitle(const QString &text)
{
  return text.isEmpty() ? text : ::html_unescape(text)
   .remove(QRegExp(" - YouTube$"))
   .simplified();
}

QString
YoutubeMrlResolver::decodeYoutubeUrl(const QUrl &url)
{ return url.toString().remove(QRegExp(".itag=\\d+$")); } // chop duplicated itag in the end

// EOF
