// nicoutil.cc
// 3/14/2012

#include "nicoutil.h"
#include <QtNetwork>

#define DEBUG "nicoutil"
#include "module/debug/debug.h"

bool
nico::login(const QString &username, const QString &password, QNetworkCookieJar *cookieJar, int retries)
{
  DOUT("enter: username =" << username << ", retries =" << retries);
  Q_ASSERT(cookieJar);
  if (!cookieJar)
    return false;
  QString url = "https://secure.nicovideo.jp/secure/login?site=niconico";
  QString post = QString("next_url=&mail=%1&password=%2").arg(username).arg(password);

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  request.setRawHeader("Referer", "https://secure.nicovideo.jp/secure/login_form");

  QNetworkAccessManager nam;
  QObject *parent = cookieJar->parent();
  nam.setCookieJar(cookieJar);
  cookieJar->setParent(parent);
  QNetworkReply *reply = nam.post(request, post.toLocal8Bit());

  QEventLoop loop;
  QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  DOUT("eventloop enter");
  loop.exec();
  DOUT("eventloop leave");

  bool ok = reply->isFinished() && reply->error() == QNetworkReply::NoError;
  if (!ok && retries-- > 0) {
    DOUT("exit: try again");
    return login(username, password, cookieJar, retries);
  }
  DOUT("exit: ret =" << ok);
  return ok;
}

// EOF
