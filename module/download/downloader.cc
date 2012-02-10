// downloader.cc
// 2/4/2011
#include "downloader.h"
#include "module/compress/qgzip.h"
#include <QtCore>
#include <QtNetwork>

//#define DEBUG "downloader"
#include "module/debug/debug.h"

// - Construction -

Downloader::Downloader(const QString &path, bool zipped, QObject *parent, QNetworkAccessManager *qnam)
  : Base(parent), qnam_(qnam), state_(OK), path_(path), zipped_(zipped)
{
  if (!qnam_)
    qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(save(QNetworkReply*)));
}

// - Download -

// See: http://stackoverflow.com/questions/5486090/qnetworkreply-wait-for-finished
void
Downloader::get(const QUrl &url, const QString &header, bool async)
{
  DOUT("enter: async =" << async << ", url =" << url.toString());
  state_ = Downloading;
  QNetworkRequest request(url);
  if (!header.isEmpty()) {
    QHash<QString, QString> h = parseHttpHeader(header);
    foreach (QString k, h.keys())
      request.setRawHeader(k.toAscii(), h[k].toAscii());
  }

  QNetworkReply *reply = qnam_->get(request);
  Q_ASSERT(reply);
  if (!reply) {
    state_ = Error;
    DOUT("exit: ERROR: invalid reply");
    return;
  }
  if (!async) {
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    DOUT("eventloop enter");
    loop.exec();
    DOUT("eventloop leave");
  }
  DOUT("exit: state =" << state_);
}

void
Downloader::post(const QUrl &url, const QByteArray &data, const QString &header, bool async)
{
  DOUT("enter: async =" << async << ", url =" << url.toString());
  state_ = Downloading;
  QNetworkRequest request(url);
  if (!header.isEmpty()) {
    QHash<QString, QString> h = parseHttpHeader(header);
    foreach (QString k, h.keys())
      request.setRawHeader(k.toAscii(), h[k].toAscii());
  }
  QNetworkReply *reply = qnam_->post(request, data);
  Q_ASSERT(reply);
  if (!reply) {
    state_ = Error;
    DOUT("exit: ERROR: invalid reply");
    return;
  }
  if (!async) {
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    DOUT("eventloop enter");
    loop.exec();
    DOUT("eventloop leave");
  }
  DOUT("exit: state =" << state_);
}

void
Downloader::save(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    state_ = Error;
    DOUT("exit: reply error: " + reply->errorString());
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
  DOUT("exit: state =" << state_);
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

// - Helper -

// See: http://stackoverflow.com/questions/2214595/how-can-i-create-a-http-post-request-with-qt-4-6-1
QByteArray
Downloader::encodeUrlParameters(const QString &params)
{
  if (params.isEmpty())
    return QByteArray();
  DOUT("enter: params =" << params);
  QString query = "?" + params;
  QUrl url(query);
  DOUT("exit: queries =" << url.queryItems());
  return url.encodedQuery();
}

QHash<QString, QString>
Downloader::parseHttpHeader(const QString &header)
{
  QHash<QString, QString> ret;
  if (header.isEmpty())
    return ret;
  DOUT("enter: header =" << header);

  QStringList l = header.split('\n');
  foreach (QString item, l) {
    int i = item.indexOf(':');
    if (i > 0) {
      QString k = item.left(i).trimmed();
      if (!k.isEmpty()) {
        QString v;
        if (i + 1 < item.size())
          v = item.mid(i + 1).trimmed();
        ret[k] = v;
      }
    }
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF
