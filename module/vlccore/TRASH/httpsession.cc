// vlccore/httpsession.cc
// 2/21/2012

#include "module/vlccore/httpsession.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/mp4codec.h"
#include <QApplication>
#include <QtCore>
#include <QtNetwork>
#include <cstring>

#include <vlc/plugins/vlc_common.h>
extern "C" {
  #include <vlc/src/modules/modules.h>
  module_t *module_find(const char *name);
} // extern "C"

//#define DEBUG "vlccore::http"
#include "module/debug/debug.h"

// - Session -

VlcHttpSession::VlcHttpSession(QObject *parent)
  : Base(parent), state_(Stopped), nam_(0), cookieJar_(0), reply_(0),
    pos_(0), size_(0), ready_(false)
{ }

VlcHttpSession::~VlcHttpSession()
{
  if (nam_)
    delete nam_;
}

bool
VlcHttpSession::isMultiMediaMimeType(const QString &contentType)
{ return !contentType.isEmpty() && !contentType.startsWith("text/", Qt::CaseInsensitive); }

void
VlcHttpSession::invalidateSize()
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
VlcHttpSession::invalidateContentType()
{
  if (reply_)
    contentType_ = reply_->header(QNetworkRequest::ContentTypeHeader).toString();
  else
    contentType_.clear();
  DOUT("contentType =" << contentType_);
}

void
VlcHttpSession::invalidateFileName()
{
  bool mp4 = contentType_.contains("mp4", Qt::CaseInsensitive);
  QString suf = mp4 ? ".mp4" : ".flv";
  QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  fileName_ = dir + "/" + escapeFileName(mediaTitle_) + suf;
}

void
VlcHttpSession::save()
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
VlcHttpSession::finish()
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
VlcHttpSession::stop()
{
  DOUT("enter");
  setState(Stopped);
  DOUT("exit");
}

void
VlcHttpSession::waitForReplyReady()
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
VlcHttpSession::tryRedirect()
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
VlcHttpSession::waitForReady()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (!ready_)
    readyCond_.wait(&m_);
  DOUT("exit");
}

void
VlcHttpSession::waitForStopped()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (isRunning())
    stoppedCond_.wait(&m_);
  DOUT("exit");
}

void
VlcHttpSession::run()
{
  DOUT("enter: url =" << url_);
  setState(Running);

  ready_ = false;
  pos_ = size_ = 0;
  if (!buffer_.isEmpty())
    buffer_.clear();
  contentType_.clear();
  fileName_.clear();

  if (mediaTitle_.isEmpty())
    mediaTitle_ = QDateTime::currentDateTime().toString(Qt::ISODate);

  nam_ = new QNetworkAccessManager;
  if (cookieJar_) {
    DOUT("use cookie jar");
    nam_->setCookieJar(cookieJar_);
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
VlcHttpSession::read(char *data, qint64 maxSize)
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
VlcHttpSession::seek(qint64 pos)
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

QString
VlcHttpSession::escapeFileName(const QString &name)
{
  QString ret = name;
  ret.remove('"');
  ret.replace('/', "／");
  ret.replace(':', "-");
  ret.replace('?', "？");
  return ret.trimmed();
}


// EOF
