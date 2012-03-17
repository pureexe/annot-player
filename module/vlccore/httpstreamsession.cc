// vlccore/httpstreamsession.cc
// 3/14/2012

#include "module/vlccore/httpstreamsession.h"
#include "module/stream/securebufferedfifostream.h"
#include "module/stream/bufferedremotestream.h"
#include "module/mediacodec/flvmerge.h"
#include "module/mediacodec/flvcodec.h"
#include "module/qtext/filesystem.h"
#include <QDesktopServices>
#include <QApplication>
#include <QtCore>
#include <QtNetwork>
#ifdef Q_OS_WIN
#  include "win/qtwin/qtwin.h"
#else
#  include <unistd.h>
#endif // Q_OS_WIN

#define DEBUG "httpstreamsession"
#include "module/debug/debug.h"

// - Progress -

namespace { // anonymous

  void sleep_(uint msecs_)
  {
#ifdef Q_OS_WIN
    QtWin::sleep(msecs_);
#else
    ::sleep(msecs_ / 1000);
#endif // Q_OS_WIN
  }

  class ProgressTask_ : public StoppableTask
  {
    enum { SleepInterval = 1000 }; // wake up every 1 second

    HttpStreamSession *session_;
    bool stop_;
  public:
    explicit ProgressTask_(HttpStreamSession *session)
      : session_(session), stop_(false) { Q_ASSERT(session_); }

    virtual void stop() ///< \override
    { stop_ = true; }

    virtual void run() ///< \override
    {
      while (!stop_ && session_->isRunning()) {
        session_->updateProgress();
        sleep_(SleepInterval);
      }
    }
  };

} // anonymous namespace

// - Construction -

HttpStreamSession::~HttpStreamSession()
{
  if (progressTask_)
    progressTask_->stop();
  foreach (InputStream *in, ins_)
    delete in;
  if (fifo_)
    delete fifo_;
  if (merger_)
    delete merger_;

  foreach (QObject *obj, deleteLater_)
    delete obj;
}

// - Properties -

qint64
HttpStreamSession::receivedSize() const
{
  if (ins_.isEmpty())
    return 0;

  qint64 ret = 0;
  foreach (InputStream *in, ins_) {
    BufferedRemoteStream *r = dynamic_cast<BufferedRemoteStream *>(in);
    Q_ASSERT(r);
    if (r)
      ret += r->availableSize();
  }
  return ret;
}

QString
HttpStreamSession::contentType() const
{
  if (ins_.isEmpty())
    return QString();

  RemoteStream *in = dynamic_cast<RemoteStream *>(ins_.first());
  Q_ASSERT(in);
  return in ? in->contentType() : QString();
}

void
HttpStreamSession::invalidateSize()
{
  if (!fifo_ || ins_.isEmpty())
    return;
  qint64 size = 0;
  foreach (InputStream *in, ins_) {
    RemoteStream *r = dynamic_cast<RemoteStream *>(in);
    Q_ASSERT(r);
    if (r)
      size += r->size();
  }

  if (size)
    fifo_->setSize(size);
  DOUT("size =" << size);
}

void
HttpStreamSession::invalidateFileName()
{
  //bool mp4 = contentType().contains("mp4", Qt::CaseInsensitive);
  QString suf = ".flv";
  QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/Annot";
  fileName_ = dir + "/" + QtExt::escapeFileName(mediaTitle()) + suf;
}

// - Actions -

void
HttpStreamSession::save()
{
  if (fileName_.isEmpty() || !fifo_ || !fifo_->availableSize())
    return;

  QFile::remove(fileName_);
  QDir().mkpath(QFileInfo(fileName_).absolutePath());

  bool ok = fifo_->writeToFile(fileName_);
  if (!ok || !FlvCodec::isFlvFile(fileName_)) {
    QFile::remove(fileName_);
    emit error(tr("download failed") + ": " + fileName_);
    return;
  }

  emit message(tr("file saved") + ": " + fileName_);
  QApplication::beep();
  emit fileSaved(fileName_);
}

void
HttpStreamSession::finish()
{
  DOUT("enter");
  qint64 size = 0;
  if (fifo_) {
    size = fifo_->availableSize();
    fifo_->setSize(size);
  }

  if (isRunning()) {
    save();
    setState(Finished);
  }

  //if (reply_ && reply_->isRunning())
  //  reply_->abort();

  emit progress(size, size);

  stoppedCond_.wakeAll();
  DOUT("exit");
}

void
HttpStreamSession::stop()
{
  DOUT("enter");
  setState(Stopped);

  if (progressTask_)
    progressTask_->stop();

  foreach (InputStream *in, ins_) {
    RemoteStream *r = dynamic_cast<RemoteStream *>(in);
    if (r)
      r->stop();
  }

  stoppedCond_.wakeAll();
  DOUT("exit");
}

void
HttpStreamSession::waitForReady()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (!ready_)
    readyCond_.wait(&m_);
  DOUT("exit");
}

void
HttpStreamSession::waitForStopped()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (isRunning())
    stoppedCond_.wait(&m_);
  DOUT("exit");
}

void
HttpStreamSession::run()
{
  DOUT("enter: urls.size =" << urls_.size() << ", urls =" << urls_);
  Q_ASSERT(!url_.isEmpty());
  if (urls_.isEmpty()) {
    DOUT("exit: empty urls");
    return;
  }
  setState(Running);
  ready_ = false;
  //progress_.clear();
  invalidateFileName();

  if (merger_) {
    delete merger_;
    merger_ = 0;
  }
  if (fifo_) {
    delete fifo_;
    fifo_ = 0;
  }
  if (!ins_.isEmpty()) {
    foreach (InputStream *in, ins_)
      delete in;
    ins_.clear();
  }

  if (mediaTitle().isEmpty())
    setMediaTitle(QDateTime::currentDateTime().toString(Qt::ISODate));

  foreach (const QUrl &url, urls_) {
    RemoteStream *in = new BufferedRemoteStream;
    //QtExt::ProgressWithId p((long)in);
    //connect(in, SIGNAL(progress(qint64,qint64)), &p, SLOT(emit_progress(qint64,qint64)));
    //connect(&p, SIGNAL(progress(qint64,qint64,long)), SLOT(updateProgressWithId(qint64,qint64,long)), Qt::QueuedConnection);
    in->setUrl(url);

    if (cookieJar())
      in->networkAccessManager()->setCookieJar(cookieJar());
    in->run();
    ins_.append(in);
    in->waitForReadyRead();
  }
  if (cookieJar()) {
    cookieJar()->setParent(0);
    deleteLater_.append(cookieJar());
  }

  //if (reply_->error() != QNetworkReply::NoError) {
  //  setState(Error);
  //  ready_ = true;
  //  readyCond_.wakeAll();
  //  stoppedCond_.wakeAll();
  //  emit error(tr("network error to access URL") + ": " + url_.toString());
  //  DOUT("exit: network error");
  //  return;
  //}

  bool ok = isMultiMediaMimeType(contentType());
  if (!ok) {
    setState(Error);
    emit error(tr("access forbidden") + ": " + urls_.first().toString());

    ready_ = true;
    readyCond_.wakeAll();
    stoppedCond_.wakeAll();
    return;
  }

  fifo_ = new SecureBufferedFifoStream;
  invalidateSize();

  updateProgress();

  FlvMerge *merger = new FlvMerge; {
    merger->setInputStreams(ins_);
    merger->setOutputStream(fifo_);
    merger->setDuration(duration_);

    connect(merger, SIGNAL(error(QString)), SIGNAL(error(QString)));
    connect(merger, SIGNAL(error(QString)), SLOT(stop()));
    //connect(merger, SIGNAL(timestampChanged(qint64)), SLOT(updateProgress()));
    merger_ = merger;
  }

  //QTimer progressTimer;
  //progressTimer.setInterval(0); // report every second
  //connect(&progressTimer, SIGNAL(timeout()), SLOT(updateProgress()), Qt::QueuedConnection);

  //progressTimer.start();
  //merger_->run();
  //progressTimer.stop();

  //ins_.reset();
  ok = merger->parse();
  if (ok) {
    duration_ = merger->duration();
    DOUT("duration =" << duration_);
  } else {
    setState(Error);
    emit error(tr("failed to parse FLV streams"));
  }

  ready_ = true;
  readyCond_.wakeAll();

  if (ok) {
    progressTask_ = new ProgressTask_(this);
    QThreadPool::globalInstance()->start(progressTask_);
  }

  if (ok) {
    ins_.reset();
    ok = merger->merge();
    if (ok)
      merger->leadOut();
    else {
      setState(Error);
      emit error(tr("failed to merge FLV streams"));
    }
  }

  if (progressTask_)
    progressTask_->stop();

  if (isRunning())
    finish();
  else
    emit tr("failed to process remote media");


  DOUT("exit");
}

// EOF

/*
// - Progress -

void
HttpStreamSession::updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id)
{
  progress_[id] = Progress(receivedBytes, totalBytes);

  receivedBytes = 0;
  totalBytes = 0;
  foreach (const Progress &p, progress_) {
    receivedBytes += p.receivedBytes;
    totalBytes += p.totalBytes;
  }
  emit progress(receivedBytes, totalBytes);
}
*/
