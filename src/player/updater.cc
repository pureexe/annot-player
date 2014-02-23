// updater.cc
// 5/27/2013

#include "updater.h"
#include "qtx/qxnetwork.h"
#include <QtCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//#define DEBUG "bilibilicodec"
#include "qtx/qxdebug.h"

#define UPDATE_URL "http://210.175.54.32/api/xml/app/version"

namespace { // anonymous
class GetThread : public QRunnable
{
  QNetworkAccessManager *nam_;
  QString url_;
public:
  GetThread(QNetworkAccessManager *nam, const QString &url) : nam_(nam), url_(url) {}
  void run() override { nam_->get(QNetworkRequest(url_)); }
};
} // anonymous namespace

// - Construction -

Updater::Updater(QObject *parent)
  : Base(parent)
{
  ncm_ = new QNetworkConfigurationManager(this);
  nam_ = new QNetworkAccessManager(this);
  connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(parseReply(QNetworkReply*)), Qt::QueuedConnection);
}

bool Updater::isOnline() const
{ return ncm_->isOnline(); }

// - Query -

void Updater::queryVersion()
{ QThreadPool::globalInstance()->start(new GetThread(nam_, UPDATE_URL)); }

void Updater::parseReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    //emit errorMessage(tr("network error, failed to resolve media URL") + ": " + url);
    DOUT("exit: network error:" << reply->errorString());
    return;
  }
  QByteArray data = reply->readAll();
  if (data.isEmpty())
    ;//emit errorMessage(tr("failed to resolve annotations from URL") + ": " + reply->url().toString());
  else
    parseData(data);
  DOUT("exit: annots.size =" << l.size());
}

void Updater::parseData(const QByteArray &data)
{
  DOUT("enter: data.size =" << data.size());
  if (data.isEmpty()) {
    DOUT("exit: empty data");
    return;
  }
  QDomDocument doc;
  doc.setContent(data);
  if (doc.isNull()) {
    DOUT("exit: invalid document root");
    return;
  }
  QDomElement root = doc.firstChildElement("grimoire");
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return;
  }
  QDomElement apps = root.firstChildElement("apps");
  if (apps.isNull()) {
    DOUT("exit: invalid apps element");
    return;
  }
  QDomElement e = apps.firstChildElement("app");
  while (!e.isNull()) {
    QString attr = e.attribute("name");
    if (attr == "ap") {
      QDomElement ts = e.firstChildElement("timestamp");
      if (!ts.isNull()) {
        QString t = ts.text().trimmed();
        long v = t.toLong();
        if (v)
          emit versionReceived(v);
      }
      break;
    }
    e = e.nextSiblingElement("app");
  }
  DOUT("exit: size =" << ret.size());
}

// EOF
