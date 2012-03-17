// vlccore/httpbufferedsession.cc
// 2/21/2012

#include "module/vlccore/httpbufferedsession.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/mp4codec.h"
#include "module/qtext/filesystem.h"
#include <QDesktopServices>
#include <QApplication>
#include <QtCore>
#include <QtNetwork>
#include <cstring>

#define DEBUG "httpbufferedsession"
#include "module/debug/debug.h"

// - Construction -

HttpBufferedSession::~HttpBufferedSession()
{ if (nam_) delete nam_; }

// - Properties -

void
HttpBufferedSession::invalidateSize()
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
HttpBufferedSession::invalidateContentType()
{
  if (reply_)
    contentType_ = reply_->header(QNetworkRequest::ContentTypeHeader).toString();
  else
    contentType_.clear();
  DOUT("contentType =" << contentType_);
}

void
HttpBufferedSession::invalidateFileName()
{
  bool mp4 = contentType_.contains("mp4", Qt::CaseInsensitive);
  QString suf = mp4 ? ".mp4" : ".flv";
  QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/Annot";
  fileName_ = dir + "/" + QtExt::escapeFileName(mediaTitle()) + suf;
}

// - Actions -

void
HttpBufferedSession::save()
{
  if (fileName_.isEmpty() || buffer_.isEmpty())
    return;
  QFile::remove(fileName_);
  QDir().mkpath(QFileInfo(fileName_).absolutePath());

  QFile file(fileName_);
  if (!file.open(QIODevice::WriteOnly)) {
    emit error(tr("cannot save to file") + ": " + fileName_);
    return;
  }

  m_.lock();
  file.write(buffer_);
  m_.unlock();
  file.close();

  if (!FlvCodec::isFlvFile(fileName_) && !Mp4Codec::isMp4File(fileName_)) {
    QFile::remove(fileName_);
    emit error(tr("download failed") + ": " + fileName_);
    return;
  }

  emit message(tr("file saved") + ": " + fileName_);
  QApplication::beep();
  emit fileSaved(fileName_);
}

void
HttpBufferedSession::finish()
{
  DOUT("enter");
  size_ = buffer_.size();

  if (isRunning()) {
    save();
    setState(Finished);
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
  setState(Stopped);
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
  if (url.isEmpty() || url != reply_->url())
    return false;
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
  if (!buffer_.isEmpty())
    buffer_.clear();
  contentType_.clear();
  fileName_.clear();

  if (mediaTitle().isEmpty())
    setMediaTitle(QDateTime::currentDateTime().toString(Qt::ISODate));

  nam_ = new QNetworkAccessManager;
  if (cookieJar()) {
    DOUT("use cookie jar");
    nam_->setCookieJar(cookieJar());
  }
  reply_ = nam_->get(QNetworkRequest(url_));
  Q_ASSERT(reply_);
  waitForReplyReady();

  if (tryRedirect()) {
    DOUT("exit: redirected");
    return;
  }

  if (reply_->error() != QNetworkReply::NoError) {
    setState(Error);
    ready_ = true;
    readyCond_.wakeAll();
    stoppedCond_.wakeAll();
    emit error(tr("network error to access URL") + ": " + url_.toString());
    DOUT("exit: network error");
    return;
  }

  if (reply_->isFinished()) {
    m_.lock();
    buffer_ = reply_->readAll();
    m_.unlock();
  }

  invalidateSize();
  invalidateContentType();
  invalidateFileName();

  if (!isMultiMediaMimeType(contentType_)) {
    setState(Error);
    emit error(tr("access forbidden") + ": " + url_.toString());
  }

  DOUT("ready: finished =" << reply_->isFinished() << ", error =" << reply_->error());
  ready_ = true;
  readyCond_.wakeAll();

  while (!reply_->isFinished() &&
         reply_->error() == QNetworkReply::NoError &&
         isRunning()) {
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
         buffer_.size() - pos_ < maxSize)
    readyReadCond_.wait(&m_);

  qint64 ret = qMin(maxSize, buffer_.size() - pos_);
  if (ret) {
    ::memcpy(data, buffer_.constData() + pos_, ret);
    pos_ += ret;
  }
  return ret;
}

bool
HttpBufferedSession::seek(qint64 pos)
{
  if (pos < 0)
    return false;
  if (pos == 0)
    return true;
  if (size_ > 0 && (pos >= size_ && pos > buffer_.size()))
    return false;

  m_.lock();
  pos_ = pos;
  m_.unlock();
  return true;
}

// EOF
