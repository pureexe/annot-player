// nicoutil.cc
// 3/14/2012

#include "lib/nicoutil/nicoutil.h"
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#define DEBUG "nicoutil"
#include "lib/debug/debug.h"

namespace { namespace detail {

  bool
  login(const QString &url, const QString &post, QNetworkCookieJar *cookieJar, int retries)
  {
    DOUT("enter: url =" << url);
    Q_ASSERT(cookieJar);
    if (!cookieJar)
      return false;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Referer", url.toAscii());

    QNetworkAccessManager nam;
    QObject *parent = cookieJar->parent();
    nam.setCookieJar(cookieJar);
    cookieJar->setParent(parent);
    QNetworkReply *reply = nam.post(request, post.toAscii());

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    DOUT("eventloop enter");
    loop.exec();
    DOUT("eventloop leave");

    bool ok = reply->isFinished() && reply->error() == QNetworkReply::NoError;
    if (!ok && retries-- > 0) {
      DOUT("exit: try again");
      return login(url, post, cookieJar, retries);
    }
    DOUT("exit: ret =" << ok);
    return ok;
  }
} } // anonymous detail

bool
nico::login(const QString &username, const QString &password, QNetworkCookieJar *cookieJar, int retries)
{
  QString url = "https://secure.nicovideo.jp/secure/login?site=niconico",
          post = QString("next_url=&mail=%1&password=%2")
                 .arg(username).arg(password);
  return detail::login(url, post, cookieJar, retries);
}

bool
bilibili::login(const QString &username, const QString &password, QNetworkCookieJar *cookieJar, int retries)
{
  QString url = "https://secure.bilibili.tv/member/index_do.php",
          post = QString("fmdo=login&dopost=login&gourl=&keeptime=604800&userid=%1&pwd=%2&keeptime=604800")
                 .arg(username).arg(password);
  return detail::login(url, post, cookieJar, retries);
}

// EOF
