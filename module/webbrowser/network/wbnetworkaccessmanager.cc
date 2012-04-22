// wbnetworkaccessmanager.cc
// 1/27/2012

#include "wbnetworkaccessmanager.h"
#include <QtCore>

//#define DEBUG "wbnetworkaccessmanager"
#include "module/debug/debug.h"

#ifdef Q_OS_LINUX
#  define RC_IMAGE_NULL IMAGEDIR "/null.png"
#else
#  define RC_IMAGE_NULL "file:///" + QCoreApplication::applicationDirPath() + "/images/null.png"
#endif Q_OS_LINUX

#define PROXY_HOST  ANNOT_PROXY_DOMAIN

namespace {
  const QString &rc_image_null_()
  {
    static QString url;
    if (url.isEmpty()) {
      url = RC_IMAGE_NULL;
#ifdef Q_WS_WIN
      url.replace("\\", "/");
#endif // Q_WS_WIN
    }
    return url;
  }
}

QStringList
WbNetworkAccessManager::supportedSites()
{
  return QStringList()
    << "akabeesoft2.com"
    << "akabeesoft2-try.com"
    << "akatsukiworks.com"
    << "wheel-soft.com"
    << "syangrila.com"
    << "collaborations2005.com"
    << "effordomsoft.com"
    << "applique-soft.com"
    << "shallotsoft.com"
    << "spermaniax.net"
    << "erogamescape.dyndns.org/~ap2/ero/toukei_kaiseki"
    << "erogamescape.dyndns.org/~ap2/ero/toukei_kaiseki/index_toukei.php";
}

QNetworkReply*
WbNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
  QUrl url = req.url();
  if (url.scheme() == "http") {
    QString host = url.host();
    if (!blockedUrls_.isEmpty() && isBlockedUrl(url)) {
      QNetworkRequest r(::rc_image_null_());
      return Base::createRequest(op, r, outgoingData);
    } else if (host == "www.nicovideo.jp" && url.path().startsWith("/watch")) {
      DOUT("nico request =" << url.toString());
      QNetworkRequest r = req;
      DOUT("nico delegate =" << transformNicoUrl(url));
      r.setUrl(transformNicoUrl(url));
      return Base::createRequest(op, r, outgoingData);
    } else if (host == "erogamescape.dyndns.org") {
      QNetworkRequest r = req;
      r.setUrl(transformEroUrl(url));
      return Base::createRequest(op, r, outgoingData);
    }
#define ELIF(_host, _transform) \
    else if (host.endsWith(_host, Qt::CaseInsensitive)) { \
      QNetworkRequest r = req; \
      r.setUrl(_transform(url)); \
      return Base::createRequest(op, r, outgoingData); \
    }
    ELIF("akabeesoft2.com", transformAb2Url)
    ELIF("syangrila.com", transformSyangrilaUrl)
    ELIF("akatsukiworks.com", transformAkatsukiWorksUrl)
    ELIF("wheel-soft.com", transformWheelSoftUrl)
    ELIF("akabeesoft2-try.com", transformAb2TryUrl)
    ELIF("collaborations2005.com", transformCo2005Url)
    ELIF("effordomsoft.com", transformEffordomSoftUrl)
    ELIF("applique-soft.com", transformAppliqueSoftUrl)
    ELIF("shallotsoft.com", transformShallotSoftUrl)
    ELIF("spermaniax.net", transformSpermaniaxUrl)
#undef ELIF
  }
  return Base::createRequest(op, req, outgoingData);
}

bool
WbNetworkAccessManager::isBlockedUrl(const QUrl &target) const
{
  QString t;
  foreach (const QUrl &blocked, blockedUrls_)
    if (target.host().endsWith(blocked.host()) &&
        ((t = blocked.path()).isEmpty() || target.path().startsWith(t)))
      return true;
  return false;
}

QUrl
WbNetworkAccessManager::transformNicoUrl(const QUrl &url)
{
  QString host = url.toString();
  if (!host.compare("nicovideo.jp", Qt::CaseInsensitive)) {
    QUrl ret = url;
    ret.setHost(PROXY_HOST);
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
  return QString("http://" PROXY_HOST "/nico/%1%2").arg(t).arg(u);
}

QUrl
WbNetworkAccessManager::transformEroUrl(const QUrl &url)
{
  QUrl ret = url;
  ret.setHost(PROXY_HOST);
  QString path = ret.path();
  //path.remove(QRegExp("^/~ap2/ero/toukei_kaiseki", Qt::CaseInsensitive));
  ret.setPath("/erogamescape" + path);
  return ret;
}

#define TRANSFORM(_host, _transform) \
  QUrl \
  WbNetworkAccessManager::_transform(const QUrl &url) \
  { \
    QUrl ret = url; \
    ret.setHost(PROXY_HOST); \
    ret.setPath("/" _host + ret.path()); \
    return ret; \
  }
  TRANSFORM("akabeesoft2", transformAb2Url)
  TRANSFORM("akabeesoft2-try", transformAb2TryUrl)
  TRANSFORM("akatsukiworks", transformAkatsukiWorksUrl)
  TRANSFORM("wheel-soft", transformWheelSoftUrl)
  TRANSFORM("syangrila", transformSyangrilaUrl)
  TRANSFORM("collaborations2005", transformCo2005Url)
  TRANSFORM("effordomsoft", transformEffordomSoftUrl)
  TRANSFORM("applique-soft", transformAppliqueSoftUrl)
  TRANSFORM("shallotsoft", transformShallotSoftUrl)
  TRANSFORM("spermaniax", transformSpermaniaxUrl)
#undef TRANSFORM

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
    if (host.endsWith("nicovideo.jp", Qt::CaseInsensitive) &&
        !host.startsWith("flapi.") && // use original nico fl api, or flash won't work properly
        !host.startsWith("msg.") && // use original nico annot URL
        !host.startsWith("smile-")) { // skip "smile-" video host
      //DOUT("nico request =" << url.toString());
      QNetworkRequest r = req;
      //DOUT("nico delegate =" << transformNicoUrl(url));
      r.setUrl(transformNicoUrl(url));
      return Base::createRequest(op, r, outgoingData);
    } else if (host.endsWith("niconicohost")) {
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
