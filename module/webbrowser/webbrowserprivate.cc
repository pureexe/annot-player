// webbrowserprivate.cc
// 1/27/2012

#include "webbrowserprivate.h"

#define DEBUG "webbrowserprivate"
#include "module/debug/debug.h"

QNetworkReply*
WBNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
  QUrl url = req.url();
  if (url.scheme() == "http") {
    QString host = url.host();
    if (host.contains("nicovideo.jp", Qt::CaseInsensitive) &&
        !host.startsWith("flapi.") && // use original nico fl api, or flash won't work properly
        !host.startsWith("msg.") && // use original nico annot URL
        !host.startsWith("smile-")) { // skip "smile-" video host
      DOUT("nico request =" << url.toString());
      QNetworkRequest r = req;
      //DOUT("nico delegate =" << transformNicoUrl(url));
      r.setUrl(transformNicoUrl(url));
      return Base::createRequest(op, r, outgoingData);
    } else if (host.contains("niconicohost")) {
      DOUT("nicohost request =" << url.toString());
      QNetworkRequest r = req;
      //DOUT("nicohost delegate =" << transformNicoHostUrl(url));
      r.setUrl(transformNicoHostUrl(url));
      return Base::createRequest(op, r, outgoingData);
    }
  }
  return Base::createRequest(op, req, outgoingData);
}

QUrl
WBNetworkAccessManager::transformNicoUrl(const QUrl &url)
{
  QString host = url.host();
  if (!host.compare("nicovideo.jp", Qt::CaseInsensitive)) {
    QString ret = url.toString();
    ret.replace(QRegExp("^http://nicovideo.jp", Qt::CaseInsensitive),
                "http://nico.galstars.net/?u=");
    return ret;
  }

  QRegExp rx("http://([\\w\\.\\-]+)\\.nicovideo\\.jp(.*)", Qt::CaseInsensitive);
  if (!rx.exactMatch(url.toString()))
    return url;
  Q_ASSERT(rx.captureCount() == 2);
  QString t = rx.cap(1),
          u = rx.cap(2);
  if (!u.isEmpty() && u[0] != '/')
    u.prepend('/');
  return QString("http://nico.galstars.net/?t=%1&u=%2").arg(t).arg(u);
}

QUrl
WBNetworkAccessManager::transformNicoHostUrl(const QUrl &url)
{
  QString host = url.host();
  if (host == "niconicohost") {
    QString ret = url.toString();
    ret.replace(QRegExp("^http://niconicohost", Qt::CaseInsensitive),
                "http://nico.galstars.net/?u=");
    return ret;
  }

  QRegExp rx("http://([\\w\\.\\-]+)\\.niconicohost(.*)");
  if (!rx.exactMatch(url.toString()))
    return url;
  Q_ASSERT(rx.captureCount() == 2);
  QString t = rx.cap(1),
          u = rx.cap(2);
  if (!u.isEmpty() && u[0] != '/')
    u.prepend('/');
  return QString("http://nico.galstars.net/?t=%1&u=%2").arg(t).arg(u);
  //return QString("http://%1.nicovideo.jp%2").arg(t).arg(u);
}

// EOF

/*
QNetworkReply*
WBNetworkAccessManager::transformRedirectedReply(QNetworkReply *reply)
{
  if (!reply)
    return 0;
  QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (url.host().contains("nico"))
  return reply;
}
*/
