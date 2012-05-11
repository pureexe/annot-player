// network/wbnetworkaccessmanager.cc
// 1/27/2012

#include "network/wbnetworkaccessmanager.h"
#include <QtNetwork/QNetworkRequest>
#include <QtCore>

//#define DEBUG "wbnetworkaccessmanager"
#include "module/debug/debug.h"

#ifdef Q_OS_LINUX
#  define RC_IMAGE_NULL IMAGEDIR "/null.png"
#else
#  define RC_IMAGE_NULL "file:///" + QCoreApplication::applicationDirPath() + "/images/null.png"
#endif // Q_OS_LINUX

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
  DOUT("url =" << req.url().toString());
  QUrl url = req.url();
  if (url.scheme() == "http") {
    QString host = url.host();
    if (!blockedUrls_.isEmpty() && isBlockedUrl(url)) {
      QNetworkRequest r(::rc_image_null_());
      return Base::createRequest(op, r, outgoingData);
    } else if (host == "www.nicovideo.jp") {
      if (url.path().startsWith("/watch/")
           //|| host == "ext.nicovideo.jp" && url.path().startsWith("/thumb_watch") // disabled, not working at the time
         ) {
        DOUT("nico request =" << url.toString());
        QNetworkRequest r = req;
        DOUT("nico delegate =" << encodeNicoUrl(url).toString());
        r.setUrl(encodeNicoUrl(url));
        return Base::createRequest(op, r, outgoingData);
      }
    } else if (host == PROXY_HOST && url.path().startsWith("/watch/")) {
      DOUT("nico watch request =" << url.toString());
      QNetworkRequest r = req;
      DOUT("nico watch delegate =" << encodeNicoWatchUrl(url).toString());
      r.setUrl(encodeNicoWatchUrl(url));
      return Base::createRequest(op, r, outgoingData);
    } else if (host == "v.youku.com") {
      if (url.path().startsWith("/player/getPlayList/VideoIDS/")) {
        DOUT("youku request =" << url.toString());
        // See: http://www.lovelucy.info/watch-youku-tudou-without-proxy.html
        QNetworkRequest r = req;
        r.setRawHeader("X-Forwarded-For", "202.108.8.82"); // cctv.com
        return Base::createRequest(op, r, outgoingData);
      }
    }
#define ELIF(_host, _encode) \
    else if (host.endsWith(_host, Qt::CaseInsensitive)) { \
      QNetworkRequest r = req; \
      r.setUrl(_encode(url)); \
      return Base::createRequest(op, r, outgoingData); \
    }
    ELIF("erogamescape.dyndns.org", encodeEroUrl)
    ELIF("akabeesoft2.com", encodeAb2Url)
    ELIF("syangrila.com", encodeSyangrilaUrl)
    ELIF("akatsukiworks.com", encodeAkatsukiWorksUrl)
    ELIF("wheel-soft.com", encodeWheelSoftUrl)
    ELIF("akabeesoft2-try.com", encodeAb2TryUrl)
    ELIF("collaborations2005.com", encodeCo2005Url)
    ELIF("effordomsoft.com", encodeEffordomSoftUrl)
    ELIF("applique-soft.com", encodeAppliqueSoftUrl)
    ELIF("shallotsoft.com", encodeShallotSoftUrl)
    ELIF("spermaniax.net", encodeSpermaniaxUrl)
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
WbNetworkAccessManager::encodeNicoUrl(const QUrl &url)
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
WbNetworkAccessManager::encodeNicoWatchUrl(const QUrl &url)
{
  QUrl ret = url;
  ret.setHost(PROXY_HOST);
  ret.setPath(ret.path().prepend("/nico/www"));
  return ret;
}

QUrl
WbNetworkAccessManager::encodeEroUrl(const QUrl &url)
{
  QUrl ret = url;
  ret.setHost(PROXY_HOST);
  QString path = ret.path();
  //path.remove(QRegExp("^/~ap2/ero/toukei_kaiseki", Qt::CaseInsensitive));
  ret.setPath("/erogamescape" + path);
  return ret;
}

#define TRANSFORM(_host, _encode) \
  QUrl \
  WbNetworkAccessManager::_encode(const QUrl &url) \
  { \
    QUrl ret = url; \
    ret.setHost(PROXY_HOST); \
    ret.setPath("/" _host + ret.path()); \
    return ret; \
  }
  TRANSFORM("akabeesoft2", encodeAb2Url)
  TRANSFORM("akabeesoft2-try", encodeAb2TryUrl)
  TRANSFORM("akatsukiworks", encodeAkatsukiWorksUrl)
  TRANSFORM("wheel-soft", encodeWheelSoftUrl)
  TRANSFORM("syangrila", encodeSyangrilaUrl)
  TRANSFORM("collaborations2005", encodeCo2005Url)
  TRANSFORM("effordomsoft", encodeEffordomSoftUrl)
  TRANSFORM("applique-soft", encodeAppliqueSoftUrl)
  TRANSFORM("shallotsoft", encodeShallotSoftUrl)
  TRANSFORM("spermaniax", encodeSpermaniaxUrl)
#undef TRANSFORM

// EOF

/*
QNetworkReply*
WbNetworkAccessManager::encodeRedirectedReply(QNetworkReply *reply)
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
      //DOUT("nico delegate =" << encodeNicoUrl(url));
      r.setUrl(encodeNicoUrl(url));
      return Base::createRequest(op, r, outgoingData);
    } else if (host.endsWith("niconicohost")) {
      DOUT("nicohost request =" << url.toString());
      QNetworkRequest r = req;
      //DOUT("nicohost delegate =" << encodeNicoHostUrl(url));
      r.setUrl(encodeNicoHostUrl(url));
      return Base::createRequest(op, r, outgoingData);
    }
  }
  return Base::createRequest(op, req, outgoingData);
}

QUrl
WbNetworkAccessManager::encodeNicoUrl(const QUrl &url)
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
WbNetworkAccessManager::encodeNicoHostUrl(const QUrl &url)
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
