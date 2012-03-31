// webbrowserprivate.cc
// 1/27/2012

#include "webbrowserprivate.h"

#define DEBUG "webbrowserprivate"
#include "module/debug/debug.h"

QNetworkReply*
WbNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
  QUrl url = req.url();
  if (url.scheme() == "http") {
    QString host = url.host();
    if (host.contains("nicovideo.jp", Qt::CaseInsensitive) &&
        host.startsWith("www.") && url.path().startsWith("/watch")) {
      //DOUT("nico request =" << url.toString());
      QNetworkRequest r = req;
      //DOUT("nico delegate =" << transformNicoUrl(url));
      r.setUrl(transformNicoUrl(url));
      return Base::createRequest(op, r, outgoingData);
    } else if (host.contains("akabeesoft2.com", Qt::CaseInsensitive)) {
      QNetworkRequest r = req;
      r.setUrl(transformAb2Url(url));
      return Base::createRequest(op, r, outgoingData);
    } else if (host == "erogamescape.dyndns.org") {
      QNetworkRequest r = req;
      r.setUrl(transformEroUrl(url));
      return Base::createRequest(op, r, outgoingData);
    }
  }
  return Base::createRequest(op, req, outgoingData);
}

QUrl
WbNetworkAccessManager::transformNicoUrl(const QUrl &url)
{
  QString host = url.toString();
  if (!host.compare("nicovideo.jp", Qt::CaseInsensitive)) {
    QUrl ret = url;
    ret.setHost("sakuradite.com");
    ret.setPath("/nicovideo" + ret.path());
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
  return QString("http://sakuradite.com/nico/%1%2").arg(t).arg(u);
}

QUrl
WbNetworkAccessManager::transformAb2Url(const QUrl &url)
{
  QUrl ret = url;
  ret.setHost("sakuradite.com");
  ret.setPath("/akabeesoft2" + ret.path());
  return ret;
}

QUrl
WbNetworkAccessManager::transformSyangrilaUrl(const QUrl &url)
{
  QUrl ret = url;
  ret.setHost("sakuradite.com");
  ret.setPath("/syangrila" + ret.path());
  return ret;
}

QUrl
WbNetworkAccessManager::transformEroUrl(const QUrl &url)
{
  QUrl ret = url;
  ret.setHost("sakuradite.com");
  QString path = ret.path();
  path.remove(QRegExp("^/~ap2/ero/toukei_kaiseki", Qt::CaseInsensitive));
  ret.setPath("/erogamescape" + path);
  return ret;
}

// EOF

/*
QNetworkReply*
WbNetworkAccessManager::transformRedirectedReply(QNetworkReply *reply)
{
  if (!reply)
    return 0;
  QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (url.host().contains("nico"))
  return reply;
}

QNetworkReply*
WbNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
  QUrl url = req.url();
  if (url.scheme() == "http") {
    QString host = url.host();
    if (host.contains("nicovideo.jp", Qt::CaseInsensitive) &&
        !host.startsWith("flapi.") && // use original nico fl api, or flash won't work properly
        !host.startsWith("msg.") && // use original nico annot URL
        !host.startsWith("smile-")) { // skip "smile-" video host
      //DOUT("nico request =" << url.toString());
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
WbNetworkAccessManager::transformNicoUrl(const QUrl &url)
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
WbNetworkAccessManager::transformNicoHostUrl(const QUrl &url)
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

*/
