// download_redirect.cc
// 6/22/2012
#include "lib/download/download.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QtCore>

#define DEBUG "download_redirect"
#include "qtx/qxdebug.h"

QUrl
dlredirect(const QUrl &url, int retries)
{
  DOUT("enter: retries =" << retries << ", url =" << url);
  QNetworkAccessManager nam;
  QNetworkRequest request(url); // always backup request to avoid being destructed
  QNetworkReply *reply = nam.get(request);

  QEventLoop loop;
  QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
  QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
  DOUT("eventloop enter: retries =" << retries);
  loop.exec();
  DOUT("eventloop leave");

  QUrl ret = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  bool ok = !ret.isEmpty() && ret != url;
  if (ok) {
    if (ret.host().isEmpty())
      ret.setHost(url.host());
    if (ret.scheme().isEmpty())
      ret.setScheme(url.scheme());
  }

  DOUT("exit");
  return ok ? ret :
         (retries-- > 0 && reply->error() != QNetworkReply::NoError) ? dlredirect(url, retries) :
         QUrl();
}

// EOF
