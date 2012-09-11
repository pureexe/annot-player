// httpbufferedsession_p.cc
// 2/21/2012

#include "module/vlchttp/httpbufferedsession_p.h"
#ifdef WITH_MODULE_MEDIACODEC
# include "module/mediacodec/flvcodec.h"
# include "module/mediacodec/mp4codec.h"
#else
# error "require module mediacodec"
#endif // WITH_MODULE_MEDIACODEC
#ifdef WITH_MODULE_MRLRESOLVER
# include "module/mrlresolver/luamrlresolver.h"
#endif // WITH_MODULE_LUARESOLVER
#include "module/qtext/filesystem.h"
#include "module/qtext/os.h"
#include <QtCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#define DEBUG "httpbufferedsession"
#include "module/debug/debug.h"


enum { StopTimeout = 1000 }; // 1 second
enum { BufferingInterval = 1000 }; // 1 second

enum { MaxTotalDownloadRetries = 3 };
enum { MaxIndividualDownloadRetries = 3 };

namespace { namespace detail {
  QtExt::SleepTimer sleep;
} } // anonymous detail

// - Construction -

HttpBufferedSession::~HttpBufferedSession()
{
  if (nam_)
    delete nam_;
}

// - Properties -

//bool
//HttpBufferedSession::isSeekable() const
//{
//  // FIXME
//  return contentType_.contains("mp4", Qt::CaseInsensitive) &&
//         url_.host().endsWith("nicovideo.jp");
//}

void
HttpBufferedSession::updateSize()
{
  if (reply_) {
    size_ = reply_->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (size_ < 0)
      size_ = 0;
  } else
    size_ = 0;
  DOUT("size =" << size_);
}

void
HttpBufferedSession::updateContentType()
{
  if (reply_)
    contentType_ = reply_->header(QNetworkRequest::ContentTypeHeader).toString();
  else
    contentType_.clear();
  DOUT("contentType =" << contentType_);
}

void
HttpBufferedSession::updateFileName()
{
  bool mp4 = contentType_.contains("mp4", Qt::CaseInsensitive);
  QString suf = mp4 ? ".mp4" : ".flv";
  fileName_ = cacheDirectory() + QDir::separator() + QtExt::escapeFileName(mediaTitle()) + suf;
}

// - Actions -

void
HttpBufferedSession::save()
{
  DOUT("enter");
  if (!isFinished()) {
    emit errorMessage(tr("downloading"));
    DOUT("exit: not finished");
    return;
  }
  if (fileName_.isEmpty() || buffer_.isEmpty() ||
      buffer_.size() < MinimumFileSize)
    return;
  QFileInfo fi(fileName_);
  QDir dir = fi.absoluteDir();
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  //for (int i = 2; QFile::exists(fileName_); i++)
  //  fileName_ = fi.absolutePath() + QDir::separator() + fi.completeBaseName() + " " + QString::number(i) + "." + fi.suffix();
  QtExt::trashOrRemoveFile(fileName_);

  QFile file(fileName_);
  if (!file.open(QIODevice::WriteOnly)) {
    emit errorMessage(tr("cannot save to file") + ": " + fileName_);
    DOUT("exit: failed to write to file:" << fileName_);
    return;
  }

  m_.lock();
  file.write(buffer_);
  m_.unlock();
  file.close();

  if (!FlvCodec::isFlvFile(fileName_) && !Mp4Codec::isMp4File(fileName_)) {
    QtExt::trashOrRemoveFile(fileName_);
    emit errorMessage(tr("download failed") + ": " + fileName_);
    DOUT("exit: failed to write to file:" << fileName_);
    return;
  }

  emit message(tr("file saved") + ": " + fileName_);
  emit fileSaved(fileName_);
  DOUT("exit: succeed");
}

void
HttpBufferedSession::finish()
{
  DOUT("enter");
  size_ = buffer_.size();

  if (isRunning()) {
    setState(Finished);
    if (isBufferSaved())
      save();
  }

  if (reply_ && reply_->isRunning())
    reply_->abort();

  emit progress(size_, size_);

  stoppedCond_.wakeAll();
  DOUT("exit");
}

void
HttpBufferedSession::stop()
{
  DOUT("enter");
  if (!isStopped()) {
    setState(Stopped);
    detail::sleep.stop();
    QtExt::sleep(StopTimeout);
  } else
    detail::sleep.stop();
  //quit();
  DOUT("exit");
}

void
HttpBufferedSession::waitForReplyReady()
{
  if (reply_) {
    //DOUT("enter");
    QEventLoop loop;
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    connect(reply_, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply_, SIGNAL(readyRead()), &loop, SLOT(quit()));
    loop.exec();
    //DOUT("exit");
  }
}

bool
HttpBufferedSession::tryRedirect()
{
  if (!reply_)
    return false;
  QUrl url = reply_->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (url.isEmpty() || url == reply_->url())
    return false;
  DOUT("redirected");
  if (url.host().isEmpty())
    url.setHost(reply_->url().host());
  if (url.scheme().isEmpty())
    url.setScheme(reply_->url().scheme());
  url_ = url;
  run();
  return true;
}

void
HttpBufferedSession::waitForReady()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (!ready_)
    readyCond_.wait(&m_);
  DOUT("exit");
}

void
HttpBufferedSession::waitForStopped()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (isRunning())
    stoppedCond_.wait(&m_);
  DOUT("exit");
}

void
HttpBufferedSession::run()
{
  DOUT("enter: url =" << url_);
  Q_ASSERT(!url_.isEmpty());
  setState(Running);

  ready_ = false;
  pos_ = size_ = 0;
  fileName_.clear();

  if (!buffer_.isEmpty())
    buffer_.clear();

  bool ok = false;
  for (int totalRetry = 0; !isStopped() && !ok && totalRetry < MaxTotalDownloadRetries; totalRetry++) {
#ifdef WITH_MODULE_MRLRESOLVER
    if (totalRetry && !originalUrl().isEmpty()) {
      DOUT("retry resolving mrl, orignal url =" << originalUrl());
      emit warning(
        QString("%1 (%2/%3): ")
        .arg(tr("failed to download, retry"))
        .arg(QString::number(totalRetry))
        .arg(QString::number(MaxTotalDownloadRetries))
        + originalUrl()
      );

      //if (totalRetry >= MinDownloadRetries) {
        enum { WaitInterval = 5 }; // wait 5 seconds
        int secs = WaitInterval + totalRetry;
        emit warning(tr("wait %1 seconds and try again").arg(QString::number(secs)) + " ...");
        DOUT(QString("wait for %1 seconds").arg(QString::number(secs)));
        detail::sleep.start(secs);
      //}
      if (isStopped()) {
        DOUT("stopped, break");
        break;
      }

      DOUT("with mrlresolver");
      LuaMrlResolver resolver_impl;
      MrlResolver &resolver = resolver_impl;
      resolver.setSynchronized(true);
      //connect(resolver, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SLOT(updateUrls(MediaInfo,QNetworkCookieJar*)));
      resolver.resolveMedia(originalUrl());
      const MediaInfo &mi = resolver.resolvedMediaInfo();
      QNetworkCookieJar *jar = resolver.resolvedCookieJar();
      if (jar) {
        setCookieJar(jar);
        //nam_->setCookieJar(jar);
        jar->setParent(nullptr); // memory leak
      }

      if (!mi.mrls.isEmpty())
        url_ = mi.mrls.first().url;
    }
#endif // WITH_MODULE_MRLRESOLVER
    for (int individualRetry = 0; !isStopped() && !ok && individualRetry < MaxIndividualDownloadRetries; individualRetry++) {
      if (individualRetry)
        emit warning(
          QString("%1 (%2/%3): ")
          .arg(tr("network error, retry"))
          .arg(QString::number(individualRetry))
          .arg(QString::number(MaxIndividualDownloadRetries))
          + url_.toString()
        );
      setState(Running);
      contentType_.clear();

      if (mediaTitle().isEmpty())
        setMediaTitle(QDateTime::currentDateTime().toString(Qt::ISODate));

      if (reply_) {
        if (reply_->isRunning())
          reply_->abort();
        reply_->deleteLater();
      }
      if (nam_) {
        DOUT("delete nam");
        //nam_->deleteLater();
        delete nam_;
      }
      nam_ = new QNetworkAccessManager;

      if (cookieJar()) {
        nam_->setCookieJar(cookieJar());
        cookieJar()->setParent(nullptr);
      }

      emit message(tr("buffering") + ": " + url_.toString());

      QNetworkRequest req(url_);
      req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
      reply_ = nam_->get(req);
      Q_ASSERT(reply_);
      waitForReplyReady();

      if (isStopped())
        break;

      if (tryRedirect()) {
        DOUT("exit: redirected");
        return;
      }

      if (reply_->error() != QNetworkReply::NoError) {
        setState(Error);
        //ready_ = true;
        //readyCond_.wakeAll();
        //stoppedCond_.wakeAll();
        emit errorMessage(tr("network error to access URL") + ": " + url_.toString());
        DOUT("continue: network error:" << reply_->error() << reply_->errorString());
      } else {
        updateContentType();
        if (!isMultiMediaMimeType(contentType_)) {
          setState(Error);
          emit errorMessage(tr("access forbidden") + ": " + url_.toString());
        } else {
          // succeed
          ok = true;
        }
      }
    }
  }

  if (isRunning() && ok) {
    if (reply_->isFinished()) {
      m_.lock();
      buffer_ = reply_->readAll();
      m_.unlock();
    }

    updateSize();
    updateFileName();
  }

  DOUT("ready: finished =" << reply_->isFinished() << ", error =" << reply_->error());
  ready_ = true;
  readyCond_.wakeAll();
  //stoppedCond_.wakeAll();

  while (isRunning() &&
         !reply_->isFinished() &&
         reply_->error() == QNetworkReply::NoError) {
    qint64 count = reply_->bytesAvailable();
    emit progress(buffer_.size(), size_);
    if (count <= 0) {
      waitForReplyReady();
      count = reply_->bytesAvailable();
      if (count <= 0)
        break;
    }
    //DOUT("count =" << count);

    m_.lock();
    buffer_.append(reply_->read(count));
    m_.unlock();
    //DOUT("readyRead");
    readyReadCond_.wakeAll();
  }

  if (isRunning() && reply_->bytesAvailable() > 0) {
    m_.lock();
    buffer_.append(reply_->readAll());
    m_.unlock();
    DOUT("readyRead");
    readyReadCond_.wakeAll();
  }

  finish();

  DOUT("exit");
}

qint64
HttpBufferedSession::read(char *data, qint64 maxSize)
{
  if (maxSize <= 0)
    return 0;

  QMutexLocker lock(&m_);
  while (isRunning() &&
         buffer_.size() - pos_ < maxSize) {
    emit buffering();
    readyReadCond_.wait(&m_);
    //if (isRunning())
    //  QtExt::sleep(BufferingInterval);
  }

  qint64 ret = qMin(maxSize, buffer_.size() - pos_);
  if (ret) {
    qMemCopy(data, buffer_.constData() + pos_, ret);
    pos_ += ret;
  }
  return ret;
}

bool
HttpBufferedSession::seek(qint64 pos)
{
  if (pos < 0)
    return false;
  if (pos == pos_)
    return true;
  if (size_ > 0 && (pos >= size_ && pos > buffer_.size()))
    return false;

  m_.lock();
  pos_ = pos;
  m_.unlock();
  return true;
}

// EOF
