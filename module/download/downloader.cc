// downloader.cc
// 2/4/2011
#include "downloader.h"
#include "module/compress/qgzip.h"
#include <QtCore>
#include <QtNetwork>

//#define DEBUG "downloader"
#include "module/debug/debug.h"

// - Construction -

Downloader::Downloader(const QString &path, bool zipped, QObject *parent)
  : Base(parent), state_(OK), path_(path), zipped_(zipped)
{
  qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(save(QNetworkReply*)));
}

// - Download -

// See: http://stackoverflow.com/questions/5486090/qnetworkreply-wait-for-finished
void
Downloader::start(const QUrl &url, bool async)
{
  state_ = Downloading;
  QNetworkReply *reply = qnam_->get(QNetworkRequest(url));
  Q_ASSERT(reply);
  if (!reply) {
    state_ = Error;
    return;
  }
  if (!async) {
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
  }
}

void
Downloader::save(QNetworkReply *reply)
{
  Q_ASSERT(reply);
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    state_ = Error;
    return;
  }
  QByteArray data = reply->readAll();
  if (zipped_) {
    QByteArray unzipped = ::gHttpUncompress(data);
    if (!unzipped.isEmpty())
      data = unzipped;
  }
  DOUT("data.size =" << data.size() << ", data =" << QString(data));
  bool ok = save(data, path_);
  state_ = ok ? OK : Error;
}

bool
Downloader::save(const QByteArray &data, const QString &path)
{
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly))
    return false;
  file.write(data);
  return file.error() == QFile::NoError;
}

// EOF
