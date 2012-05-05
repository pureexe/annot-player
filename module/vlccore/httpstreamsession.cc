// vlccore/httpstreamsession.cc
// 3/14/2012

#include "module/vlccore/httpstreamsession.h"
#ifdef WITH_MODULE_STREAM
#  include "module/stream/bufferedremotestream.h"
#  include "module/stream/securebufferedfifostream.h"
#else
#  error "stream module is required"
#endif // WITH_MODULE_STREAM
#ifdef WITH_MODULE_MEDIACODEC
#  include "module/mediacodec/flvcodec.h"
#  include "module/mediacodec/flvmerge.h"
#else
#  error "mediacodec module is required"
#endif // WITH_MODULE_MEDIACODEC
#ifdef WITH_MODULE_MRLRESOLVER
#  include "module/mrlresolver/luamrlresolver.h"
#endif // WITH_MODULE_LUARESOLVER
#include "module/qtext/htmltag.h"
#include "module/qtext/filesystem.h"
#include "module/qtext/os.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore>

#define DEBUG "httpstreamsession"
#include "module/debug/debug.h"

enum { MaxDownloadRetries = 5 };

// - Progress -

namespace { // anonymous

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
        QtExt::sleep(SleepInterval);
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

  //foreach (QObject *obj, deleteLater_)
  //  delete obj;

  if (nam_)
    delete nam_;
}

// - Properties -

qint64
HttpStreamSession::availableDuration() const
{ return merger_ ? merger_->timestamp() : 0; }

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
HttpStreamSession::updateSize()
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
HttpStreamSession::updateFileName()
{
  //bool mp4 = contentType().contains("mp4", Qt::CaseInsensitive);
  QString suf = ".flv";
  fileName_ = cachePath() + FILE_PATH_SEP + QtExt::escapeFileName(mediaTitle()) + suf;
}

// - Actions -

void
HttpStreamSession::save()
{
  DOUT("enter");
  if (!isFinished()) {
    emit error(tr("downloading"));
    DOUT("exit: not finished");
    return;
  }
  if (fileName_.isEmpty() || !fifo_ || !fifo_->availableSize()) {
    DOUT("exit: empty fileName or fifo");
    return;
  }

  QFile::remove(fileName_);
  QDir dir = QFileInfo(fileName_).absoluteDir();
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  bool ok = fifo_->writeFile(fileName_);
  if (!ok || !FlvCodec::isFlvFile(fileName_)) {
    QFile::remove(fileName_);
    emit error(tr("download failed") + ": " + fileName_);
    DOUT("exit: fifo failed to write to file");
    return;
  }

  ok = FlvCodec::updateFlvFileMeta(fileName_);
  if (!ok)
    DOUT("warning: failed to update FLV meta:" << fileName_);

  emit message(tr("file saved") + ": " + fileName_);
  emit fileSaved(fileName_);
  DOUT("exit");
}

void
HttpStreamSession::finish()
{
  DOUT("enter");
  qint64 size = 0;
  if (fifo_) {
    fifo_->finish();
    size = fifo_->size();
  }

  if (isRunning()) {
    setState(Finished);
    if (isBufferSaved())
      save();
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

  emit stopped();
  quit();
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
  Q_ASSERT(!urls_.isEmpty());
  if (urls_.isEmpty()) {
    DOUT("exit: empty urls");
    return;
  }
  setState(Running);
  ready_ = false;
  //progress_.clear();
  updateFileName();

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

  if (nam_)
    nam_->deleteLater();

  nam_ = new QNetworkAccessManager;

  if (cookieJar()) {
    nam_->setCookieJar(cookieJar());
    cookieJar()->setParent(0);
  }

  bool ok = false;

  for (int j = 0; j < MaxDownloadRetries && !ok; j++) {
    DOUT("for: j =" << j);
    int count = 0;
    foreach (const QUrl &url, urls_) {
      emit message(
        (tr("preparing") + HTML_STYLE_OPEN(color:orange) " %1/%2: " HTML_STYLE_CLOSE())
          .arg(QString::number(++count))
          .arg(QString::number(urls_.size()))
        + url.toString()
      );
      DOUT("foreach: count =" << count);

      ok = false;
      for (int i = 0; i < MaxDownloadRetries && !ok; i++) {
        DOUT("for: i =" << i);
        RemoteStream *in = new BufferedRemoteStream(nam_);
        in->setUrl(url);
        if (i == 0)
          in->request().setPriority(QNetworkRequest::NormalPriority);
        in->run();
        ins_.append(in);
        in->waitForReady();
        QString type = in->contentType();
        ok = isMultiMediaMimeType(type);
        if (!ok) {
          DOUT("invalid contentType =" << type << ", url =" << url.toString());
          ins_.removeLast();
          in->stop();
          in->deleteLater();
          emit warning(
            QString("%1 (%2/%3): ")
            .arg(tr("failed to fetch HTTP header, retry"))
            .arg(QString::number(i+1))
            .arg(QString::number(MaxDownloadRetries))
            + url.toString()
          );
        }
      }

      if (!ok) {
        foreach (InputStream *is, ins_) {
          RemoteStream *rs = dynamic_cast<RemoteStream *>(is);
          rs->stop();
          rs->deleteLater();
        }
        ins_.clear();

        DOUT("WARNING: failed to download url:" << url.toString());
        break;
      }
    }
    if (!ok && !originalUrl().isEmpty() && j != MaxDownloadRetries -1) {
#ifdef WITH_MODULE_MRLRESOLVER
      DOUT("retry resolving mrl, orignal url =" << originalUrl());
      emit warning(
        QString("%1 (%2/%3): ")
        .arg(tr("failed to fetch part %1, retry").arg(QString::number(count)))
        .arg(QString::number(j+1))
        .arg(QString::number(MaxDownloadRetries))
        + originalUrl()
      );

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
        nam_->setCookieJar(jar);
        jar->setParent(0); // memory leak
      }

      urls_.clear();
      bool updateDuration = !duration_;
      foreach (const MrlInfo &i, mi.mrls) {
        urls_.append(i.url);
        if (updateDuration)
         duration_ += i.duration;
      }
      if (urls_.isEmpty())
#endif // WITH_MODULE_MRLRESOLVER
      {
        DOUT("RESOLVE MEDIA FAILURE");
        break;
      }
    }
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

 if (ins_.isEmpty()) {
    DOUT("access forbidden");
    setState(Error);
    emit error(tr("access forbidden") + ": " + urls_.first().toString());

    if (nam_) {
      nam_->deleteLater();
      nam_ = 0;
    }

    ready_ = true;
    readyCond_.wakeAll();
    stoppedCond_.wakeAll();
    DOUT("exit: error caused by invalid content type");
    return;
  }

  if (fifo_)
    delete fifo_;
  fifo_ = new SecureBufferedFifoStream;
  updateSize();

  updateProgress();

  if (merger_)
    delete merger_;

  merger_ = new FlvMerge; {
    merger_->setInputStreams(ins_);
    merger_->setOutputStream(fifo_);
    merger_->setDuration(duration_);

    connect(merger_, SIGNAL(error(QString)), SIGNAL(error(QString)));
    connect(merger_, SIGNAL(error(QString)), SLOT(stop()));
    //connect(merger_, SIGNAL(timestampChanged(qint64)), SLOT(updateProgress()));
  }

  //QTimer progressTimer;
  //progressTimer.setInterval(0); // report every second
  //connect(&progressTimer, SIGNAL(timeout()), SLOT(updateProgress()), Qt::QueuedConnection);

  //progressTimer.start();
  //merger_->run();
  //progressTimer.stop();

  //ins_.reset();

  emit message(tr("buffering ..."));

  ok = merger_->parse();
  if (ok) {
    duration_ = merger_->duration();
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
    ok = merger_->merge();
    if (ok)
      merger_->finish();
    else {
      setState(Error);
      emit error(tr("failed to merge FLV streams"));
    }
  }

  if (progressTask_)
    progressTask_->stop();

  if (isRunning()) {
    finish();

    DOUT("exec after finished");
  } else
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
