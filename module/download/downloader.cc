// downloader.cc
// 2/4/2011
#include "downloader.h"
#ifdef WITH_MODULE_COMPRESS
#  include "module/compress/qgzip.h"
#endif // WITH_MODULE_COMPRESS
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QStringList>

//#define DEBUG "downloader"
#include "module/debug/debug.h"

namespace { // anonymous
  inline QString filterHost_(const QString &host)
  { return host == ANNOT_PROXY_DOMAIN ? QString() : host; }
} // anonymous namespace

// - Construction -

//namespace { namespace shared_ { // anonymous
//  QMutex eventLoopMutex; // very strange: QEventLoop in diff threads would crash.
//} } // anonymous namespace

void
Downloader::init()
{
  nam_ = new QNetworkAccessManager(this);
  connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(save(QNetworkReply*)));

  DownloaderController *c = DownloaderController::globalController();
  connect(this, SIGNAL(message(QString)), c, SIGNAL(message(QString)));
  connect(this, SIGNAL(error(QString)), c, SIGNAL(error(QString)));
  connect(this, SIGNAL(warning(QString)), c, SIGNAL(warning(QString)));
  connect(this, SIGNAL(notification(QString)), c, SIGNAL(notification(QString)));
  connect(c, SIGNAL(aborted()), SLOT(abort()));
}

void
Downloader::abort()
{ if (reply_) reply_->abort(); }

// - Download -

void
Downloader::get(const QNetworkRequest &req, bool async, int retries)
{
  DOUT("enter: async =" << async << ", url =" << req.url().toString() << ", retries =" << retries);
  state_ = Downloading;
  QUrl url = req.url();
  emit message(tr("fetching") + ": " +
    ::filterHost_(url.host()) + url.path()
  );

  reply_ = nam_->get(req);
  connect(reply_, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));

  // See: http://www.developer.nokia.com/Community/Wiki/CS001432_-_Handling_an_HTTP_redirect_with_QNetworkAccessManager
  if (!async) {
    QEventLoop loop;
    connect(reply_, SIGNAL(finished()), &loop, SLOT(quit()), Qt::QueuedConnection);
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()), Qt::QueuedConnection);
    DOUT("eventloop enter");
    loop.exec();
    DOUT("eventloop leave");

    reply_ = 0;

    while (retries-- > 0 && state_ != OK) {
      emit warning(QString("%1 (%2:%3--)")
        .arg(tr("try again"))
        .arg(tr("retries"))
        .arg(QString::number(retries +1))
      );
      get(req, false, retries); // async = false
    }
  }

  DOUT("exit: state =" << state_);
}

// See: http://stackoverflow.com/questions/5486090/qnetworkreply-wait-for-finished
void
Downloader::get(const QUrl &url, const QString &header, bool async, int retries)
{
  DOUT("enter: async =" << async << ", url =" << url.toString() << ", retries =" << retries);
  state_ = Downloading;
  emit message(tr("fetching") + ": " +
    ::filterHost_(url.host()) + url.path()
  );

  QNetworkRequest request(url);
  if (!header.isEmpty()) {
    QHash<QString, QString> h = parseHttpHeader(header);
    for (QHash<QString, QString>::ConstIterator
         p = h.begin(); p != h.end(); ++p)
      request.setRawHeader(p.key().toAscii(), p.value().toAscii());
  }

  reply_ = nam_->get(request);
  connect(reply_, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));

  // See: http://www.developer.nokia.com/Community/Wiki/CS001432_-_Handling_an_HTTP_redirect_with_QNetworkAccessManager
  if (!async) {
    QEventLoop loop;
    connect(reply_, SIGNAL(finished()), &loop, SLOT(quit()), Qt::QueuedConnection);
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()), Qt::QueuedConnection);
    DOUT("eventloop enter");
    loop.exec();
    DOUT("eventloop leave");

    reply_ = 0;

    while (retries-- > 0 && state_ != OK) {
      emit warning(QString("%1 (%2:%3--)")
        .arg(tr("try again"))
        .arg(tr("retries"))
        .arg(QString::number(retries +1))
      );
      get(url, header, false, retries); // async = false
    }
  }

  DOUT("exit: state =" << state_);
}

void
Downloader::post(const QUrl &url, const QByteArray &data, const QString &header,
                 bool async, int retries)
{
  DOUT("enter: async =" << async << ", url =" << url.toString() << ", retries =" << retries);
  state_ = Downloading;

  emit message(tr("fetching") + ": " +
    ::filterHost_(url.host()) + url.path()
  );

  QNetworkRequest request(url);
  if (!header.isEmpty()) {
    QHash<QString, QString> h = parseHttpHeader(header);
    for (QHash<QString, QString>::ConstIterator
         p = h.begin(); p != h.end(); ++p)
      request.setRawHeader(p.key().toAscii(), p.value().toAscii());
  }
  reply_ = nam_->post(request, data);
  connect(reply_, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));

  if (!async) {
    QEventLoop loop;
    connect(reply_, SIGNAL(finished()), &loop, SLOT(quit()), Qt::QueuedConnection);
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()), Qt::QueuedConnection);
    DOUT("eventloop enter");
    loop.exec();
    DOUT("eventloop leave");

    reply_ = 0;

    while (retries-- > 0 && state_ != OK) {
      emit warning(QString("%1 (%2:%3--)")
        .arg(tr("try again"))
        .arg(tr("retries"))
        .arg(QString::number(retries +1))
      );
      post(url, data, header, false, retries); // async = false
    }
  }
  DOUT("exit: state =" << state_);
}

// See: http://www.developer.nokia.com/Community/Wiki/CS001432_-_Handling_an_HTTP_redirect_with_QNetworkAccessManager
bool
Downloader::tryRedirect(QNetworkReply *reply)
{
  DOUT("enter: http error code =" << reply->error());
  Q_ASSERT(reply);
  QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  bool ret = !url.isEmpty() && url != reply->url();
  if (ret) {
    if (fileName_.isEmpty())
      state_ = OK;
    else
      get(url, QString(), false); // header = null, async = false
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

void
Downloader::save(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);

  reply_ = 0;
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    state_ = Error;
    DOUT("exit: reply error: " + reply->errorString());
    return;
  }
  if (tryRedirect(reply)) {
    DOUT("exit: redirected URL");
    return;
  }
  if (fileName_.isEmpty()) {
    state_ = OK;
    DOUT("exit: no fileName required");
    return;
  }
  QByteArray data = reply->readAll();
  DOUT("raw data size =" << data.size());

  if (reply->rawHeader("Content-Encoding") == "gzip") {
#ifdef WITH_MODULE_COMPRESS
    QByteArray unzipped = ::gHttpUncompress(data);
    if (!unzipped.isEmpty())
      data = unzipped;
#else
    DOUT("warning: replied with gzip encoding");
#endif // WITH_MODULE_COMPRESS
  }
  DOUT("data.size =" << data.size() << ", data =" << QString(data.left(50)) << "...");
  bool ok = save(data, fileName_);
  state_ = ok ? OK : Error;
  if (ok)
    emit finished();
  else
    emit error(tr("failed to save file") + ": " + fileName_);
  DOUT("exit: state =" << state_);
}

bool
Downloader::save(const QByteArray &data, const QString &fileName)
{
  QFile file(fileName);
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
  foreach (const QString &item, l) {
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
