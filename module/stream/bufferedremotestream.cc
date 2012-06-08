// bufferedremotestream.cc
// 2/15/2012

#include "module/stream/bufferedremotestream.h"
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <cstring>

//#define DEBUG "bufferedremotestream"
#include "module/debug/debug.h"

// - Construction -

BufferedRemoteStream::~BufferedRemoteStream()
{
  // Because careful: if nam is deleted before this, reply_ will become invalid.
  if (reply_ && reply_->isRunning()) {
    reply_->abort();
    reply_->deleteLater();
  }
}

// - I/O -

qint64
BufferedRemoteStream::tryRead(char *data, qint64 maxSize)
{
  Q_ASSERT(data);
  DOUT("enter: maxSize =" << maxSize << ", pos =" << pos_ << ", specifiedSize =" << Base::size());
  if (maxSize <= 0) {
    DOUT("exit: empty maxSize");
    return 0;
  }
  if (!reply_) {
    DOUT("exit: not running");
    return 0;
  }

  if (reply_->isFinished()) {
    DOUT("finished");
    if (isRedirected()) {
      redirect();
      DOUT("exit: redirect");
      return tryRead(data, maxSize);
    }
    if (data_.isEmpty()) {
      DOUT("exit: return empty data");
      return 0;
    }
    m_.lock();
    qint64 ret = qMin(data_.size() - pos_, maxSize);
    if (ret > 0) {
      ::memcpy(data, data_.constData() + pos_, ret);
      pos_ += ret;
    } else
      ret = 0;
    m_.unlock();
    DOUT("exit: ret =" << ret);
    return ret;
  }

  qint64 leftSize = data_.size() - pos_;
  if (maxSize <= leftSize) {
    m_.lock();
    ::memcpy(data, data_.constData() + pos_, maxSize);
    pos_ += maxSize;
    m_.unlock();
    DOUT("exit: ret =" << maxSize);
    return maxSize;
  }

  qint64 count;
  if (stopped_ ||
      (count = reply_->bytesAvailable()) < maxSize - leftSize) {
    DOUT("exit: ret = 0, insufficient available data");
    return 0;
  }

  qint64 ret = qMin(maxSize, count + leftSize);
  DOUT("received size =" << count);
  if (ret > 0) {
    m_.lock();
    data_.append(reply_->read(count));
    ::memcpy(data, data_.constData() + pos_, ret);
    pos_ += ret;
    m_.unlock();
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
BufferedRemoteStream::read(char *data, qint64 maxSize)
{
  Q_ASSERT(data);
  DOUT("enter: maxSize =" << maxSize << ", pos =" << pos_ << ", specifiedSize =" << Base::size());
  if (maxSize <= 0) {
    DOUT("exit: empty maxSize");
    return 0;
  }
  if (!reply_)
    run();
  //else {
  //  qint64 specifiedSize = Base::size();
  //  if (specifiedSize) {
  //    qint64 leftSize = specifiedSize - pos_;
  //    if (leftSize <= 0)
  //      return 0;
  //    if (leftSize < maxSize)
  //      maxSize = leftSize;
  //  }
  //}

  if (reply_->isFinished()) {
    DOUT("finished");
    if (isRedirected()) {
      redirect();
      DOUT("exit: redirect");
      return read(data, maxSize);
    }
    if (data_.isEmpty()) {
      DOUT("exit: return empty data");
      return 0;
    }
    m_.lock();
    qint64 ret = qMin(data_.size() - pos_, maxSize);
    if (ret > 0) {
      ::memcpy(data, data_.constData() + pos_, ret);
      pos_ += ret;
    } else
      ret = 0;
    m_.unlock();
    DOUT("exit: ret =" << ret);
    return ret;
  }

  qint64 leftSize = data_.size() - pos_;
  if (maxSize <= leftSize) {
    m_.lock();
    ::memcpy(data, data_.constData() + pos_, maxSize);
    pos_ += maxSize;
    m_.unlock();
    DOUT("exit: ret =" << maxSize);
    return maxSize;
  }

  qint64 count;
  while ((count = reply_->bytesAvailable()) < maxSize - leftSize &&
         reply_->isRunning() && !stopped_) {
    waitForReadyRead();
    if (isRedirected()) {
      redirect();
      DOUT("exit: redirect");
      return read(data, qMin(maxSize, count + leftSize));
    }
    DOUT("received size =" << count);
  }

  qint64 ret = qMin(maxSize, count + leftSize);
  DOUT("received size =" << count);
  if (ret > 0) {
    m_.lock();
    data_.append(reply_->read(count));
    ::memcpy(data, data_.constData() + pos_, ret);
    pos_ += ret;
    m_.unlock();
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

void
BufferedRemoteStream::stop()
{
  stopped_ = true;
  if (reply_ && reply_->isRunning())
    reply_->abort();
  emit stopped();
}

void
BufferedRemoteStream::run()
{
  DOUT("enter");
  Q_ASSERT(networkAccessManager());
  if (!data_.isEmpty()) {
    m_.lock();
    data_.clear();
    m_.unlock();
  }
  if (!redirectUrl_.isEmpty()) {
    if (reply_)
      reply_->deleteLater();
    QUrl url = redirectUrl_;
    redirectUrl_.clear();
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    reply_ = networkAccessManager()->get(req);
    if (bufferSize_ > 0)
      reply_->setReadBufferSize(bufferSize_);
  //} else if (reply_) {
  //  DOUT("exit; reply already created");
  //  return;
  } else {
    DOUT("url =" << request().url());
    reply_ = networkAccessManager()->get(request());
    if (bufferSize_ > 0)
      reply_->setReadBufferSize(bufferSize_);
  }
  connect(reply_, SIGNAL(readyRead()), SLOT(updateSize()));
  connect(reply_, SIGNAL(finished()), SLOT(finish()));
  connect(reply_, SIGNAL(finished()), SIGNAL(finished()));
  connect(reply_, SIGNAL(readyRead()), SIGNAL(readyRead()));
  connect(reply_, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
  connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(networkError()));

  DOUT("exit");
}

void
BufferedRemoteStream::finish()
{
  DOUT("enter");
  if (reply_ && !isRedirected()) {
    m_.lock();
    DOUT("append remaining data");
    data_.append(reply_->readAll());
    m_.unlock();
  } else
    DOUT("reply is null or redirected");
  DOUT("exit");
}

// - Properties -

qint64
BufferedRemoteStream::availableSize() const
{
  return reply_ ? data_.size() + reply_->bytesAvailable()
                : data_.size();
}

QString
BufferedRemoteStream::contentType() const
{
  return reply_ ? reply_->header(QNetworkRequest::ContentTypeHeader).toString() :
                  QString();
}

// - Wait -

void
BufferedRemoteStream::waitForFinished()
{
  DOUT("enter");
  if (!isRunning()) {
    DOUT("exit: not running");
    return;
  }

  QEventLoop loop;
  connect(reply_, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(this, SIGNAL(stopped()), &loop, SLOT(quit()));
  connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec();
  DOUT("exit");
}

void
BufferedRemoteStream::waitForReadyRead()
{
  DOUT("enter");
  if (!isRunning()) {
    DOUT("exit: not running");
    return;
  }

  Q_ASSERT(reply_);

  QEventLoop loop;
  connect(reply_, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(reply_, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(this, SIGNAL(stopped()), &loop, SLOT(quit()));
  connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(this, SIGNAL(readyRead()), &loop, SLOT(quit()));
  loop.exec();
  DOUT("exit");
}

// - I/O -

bool
BufferedRemoteStream::seek(qint64 pos)
{
  QMutexLocker ml(&m_);

  qint64 size = Base::size();

  if (size && pos >= size)
    return false;
  else {
    pos_ = pos;
    return true;
  }
}

qint64
BufferedRemoteStream::skip(qint64 count)
{
  QMutexLocker ml(&m_);
  qint64 size = Base::size();
  qint64 ret;
  if (size && pos_ + count >= size) {
     ret = size - pos_;
     pos_ = size;
  } else {
    pos_ += count;
    ret = count;
  }
  return ret;
}

QByteArray
BufferedRemoteStream::readAll()
{
  if (!reply_)
    run();
  if (isRunning())
    waitForFinished();
  if (isRedirected()) {
    redirect();
    return readAll();
  }
  return data_;
}

// - Redirection -

bool
BufferedRemoteStream::isRedirected() const
{
  if (!reply_)
    return false;
  QUrl url = reply_->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  return !url.isEmpty() && url != reply_->url();
}

bool
BufferedRemoteStream::redirect()
{
  DOUT("enter");
  if (!reply_) {
    DOUT("exit: reply is null");
    return false;
  }

  QUrl url = reply_->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (url.isEmpty() || url == reply_->url()) {
    DOUT("exit: redirection attribute doesn't exist");
    return false;
  }
  if (url.host().isEmpty())
    url.setHost(reply_->url().host());
  if (url.scheme().isEmpty())
    url.setScheme(reply_->url().scheme());
  redirectUrl_ = url;
  DOUT("redirectUrl =" << redirectUrl_);
  run();
  DOUT("exit: ret = true");
  return true;
}

void
BufferedRemoteStream::updateSize()
{
  //DOUT("enter");
  if (reply_ && !Base::size()) {
    qint64 contentLength = reply_->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (contentLength > 0) {
      DOUT("reset stream size =" << contentLength);
      setSize(contentLength);
    }
  }
  //DOUT("exit");
}

void
BufferedRemoteStream::networkError()
{ emit error(tr("network error, failed to download remote resource")); }

bool
BufferedRemoteStream::writeFile(const QString &path)
{
  DOUT("enter: fileName =" << path);
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    DOUT("exit: failed to write to file:" << path);
    return false;
  }

  bool ok = file.write(data_) == data_.size();

  DOUT("exit: ret =" << ok);
  return ok;
}

// EOF
