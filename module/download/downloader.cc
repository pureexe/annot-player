// downloader.cc
// 2/4/2011
#include "downloader.h"
#include "module/compress/qgzip.h"
#include <QtCore>
#include <QtNetwork>

//#define DEBUG "downloader"
#include "module/debug/debug.h"

// - Construction -

Downloader::Downloader(const QString &path, bool zipped, QObject *parent, QNetworkAccessManager *nam)
  : Base(parent), nam_(nam), state_(OK), path_(path), zipped_(zipped)
{
  if (!nam_)
    nam_ = new QNetworkAccessManager(this);
  connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(save(QNetworkReply*)));
}

// - Download -

// See: http://stackoverflow.com/questions/5486090/qnetworkreply-wait-for-finished
void
Downloader::get(const QUrl &url, const QString &header, bool async, int retries)
{
  DOUT("enter: async =" << async << ", url =" << url.toString() << ", retries =" << retries);
  state_ = Downloading;
  QNetworkRequest request(url);
  if (!header.isEmpty()) {
    QHash<QString, QString> h = parseHttpHeader(header);
    foreach (QString k, h.keys())
      request.setRawHeader(k.toAscii(), h[k].toAscii());
  }

  QNetworkReply *reply = nam_->get(request);
  Q_ASSERT(reply);
  if (!reply) {
    state_ = Error;
    DOUT("exit: ERROR: invalid reply");
    return;
  }

  // See: http://www.developer.nokia.com/Community/Wiki/CS001432_-_Handling_an_HTTP_redirect_with_QNetworkAccessManager
  if (!async) {
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    DOUT("eventloop enter");
    loop.exec();
    DOUT("eventloop leave");

    while (state_ != OK && retries-- > 0)
      get(url, header, false, retries); // async = false
  }

  DOUT("exit: state =" << state_);
}

void
Downloader::post(const QUrl &url, const QByteArray &data, const QString &header,
                 bool async, int retries)
{
  DOUT("enter: async =" << async << ", url =" << url.toString() << ", retries =" << retries);
  state_ = Downloading;
  QNetworkRequest request(url);
  if (!header.isEmpty()) {
    QHash<QString, QString> h = parseHttpHeader(header);
    foreach (QString k, h.keys())
      request.setRawHeader(k.toAscii(), h[k].toAscii());
  }
  QNetworkReply *reply = nam_->post(request, data);
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

    while (state_ != OK && retries-- > 0)
      post(url, data, header, false, retries); // async = false
  }
  DOUT("exit: state =" << state_);
}

// See: http://www.developer.nokia.com/Community/Wiki/CS001432_-_Handling_an_HTTP_redirect_with_QNetworkAccessManager
void
Downloader::redirect(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (!url.isEmpty() && url != reply->url())
    get(url, QString(), false); // header = QString(), async = false
  DOUT("exit");
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
  DOUT("raw data size =" << data.size());

  if (data.isEmpty()) {
    redirect(reply);
    DOUT("exit: try redirecting URL");
    return;
  }

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
