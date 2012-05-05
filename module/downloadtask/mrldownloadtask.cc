// mrldownloadtask.cc
// 2/20/2012
#include "module/downloadtask/mrldownloadtask.h"
#if !defined WITH_MODULE_MRLRESOLVER || !defined WITH_MODULE_STREAM || !defined WITH_MODULE_MEDIACODEC
#  error "required modules missing"
#endif // !WITH_MODULE_
#include "module/mrlresolver/mrlresolvermanager.h"
#include "module/stream/bufferedremotestream.h"
#include "module/stream/bufferedstreampipe.h"
#include "module/stream/fileoutputstream.h"
#include "module/mediacodec/mp4codec.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/flvmerge.h"
#include "module/qtext/filesystem.h"
#include "module/qtext/network.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QDir>
#include <QtCore/QTimer>

#define DEBUG "mrldownloadtask"
#include "module/debug/debug.h"

enum { MaxDownloadRetries = 5 };

// - Helper -

bool
MrlDownloadTask::isMultiMediaMimeType(const QString &contentType)
{ return !contentType.isEmpty() && !contentType.startsWith("text/", Qt::CaseInsensitive); }

// - Construction -

MrlDownloadTask::~MrlDownloadTask()
{
  stop();
  if (resolver_)
    delete resolver_;
}

// - Task -

void
MrlDownloadTask::reset()
{
  retries_ = MaxDownloadRetries;
  stopped_ = false; Base::reset();
}

void
MrlDownloadTask::stop()
{
  stopped_ = true;
  setState(Stopped);
  emit stopped();
}

void
MrlDownloadTask::run()
{
  DOUT("enter");
  if (stopped_) {
    DOUT("exit: stopped");
    return;
  }
  setState(Downloading);
  progress_.clear();

  if (resolver_)
    resolver_->deleteLater();
  resolver_ = new MrlResolverManager;
  resolver_->setSynchronized(true);
  connect(resolver_, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(resolver_, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SLOT(downloadMedia(MediaInfo,QNetworkCookieJar*)));
  bool ok = resolver_->resolveMedia(url());
  if (!ok) {
    setState(Error);
    emit error(tr("failed to download from URL") + ": " + url());
  }
  DOUT("exit");
}

// - Download -

void
MrlDownloadTask::downloadMedia(const MediaInfo &mi, QNetworkCookieJar *jar)
{
  DOUT("enter");
  if (stopped_) {
    DOUT("exit: stopped");
    return;
  }

  QDir dir(downloadPath());
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  QString title = mi.title;
  if (title.isEmpty())
    title = tr("unknown");
  setTitle(title);

  if (!mi.refurl.isEmpty())
    setUrl(mi.refurl);

  switch (mi.mrls.size()) {
  case 0: emit error(tr("failed to resolve media URL") +": " + mi.refurl); break;
  case 1: downloadSingleMedia(mi, jar); break;
  default: downloadMultipleMedia(mi, jar);
  }

  DOUT("exit");
}

// - Download -

void
MrlDownloadTask::downloadSingleMedia(const MediaInfo &mi, QNetworkCookieJar *jar)
{
  DOUT("enter");

  QString title = mi.title;
  if (title.isEmpty())
    title = tr("unknown");
  setTitle(title);

  // - Start
  Q_ASSERT(mi.mrls.size() == 1);
  if (mi.mrls.isEmpty()) {
    setFileName(QString());
    setState(Error);
    DOUT("exit: empty mrl");
    return;
  }

  if (stopped_) {
    DOUT("exit: stopped");
    return;
  }
  // - Input
  QNetworkAccessManager *nam = new QNetworkAccessManager; // on the heap so that it is the last to delete
  if (jar)
    nam->setCookieJar(jar);
  BufferedRemoteStream in_impl(nam);
  nam->setParent(&in_impl);

  RemoteStream &in = in_impl;
  connect(&in, SIGNAL(progress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
  connect(&in, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(this, SIGNAL(stopped()), &in, SLOT(stop()));

  QString realUrl = mi.mrls.first().url;
  DOUT("realUrl =" << realUrl);

  in.setUrl(realUrl);
  in.run();
  in.waitForReady();

  if (stopped_) {
    stop();
    DOUT("exit: stopped");
    return;
  }

  QString contentType = in.contentType();
  if (!isMultiMediaMimeType(contentType)) {
    setState(Error);
    emit error(tr("access denied to download URL") + ": " + realUrl);
    in.stop();
    return;
  }

  // Output
  QString suf = contentType.contains("mp4", Qt::CaseInsensitive) ? ".mp4" : ".flv";

  FileOutputStream out;
  QString name = QtExt::escapeFileName(title);
  QString tmpFile = downloadPath() + FILE_PATH_SEP "_" + name + suf;
  for (int i = 2; QFile::exists(tmpFile); i++)
    tmpFile = downloadPath() + FILE_PATH_SEP "_" + name + " " + QString::number(i) + suf;

  DOUT("fileName =" << tmpFile);
  setFileName(tmpFile);

  out.setFileName(tmpFile);
  if (!out.open()) {
    setState(Error);
    emit error(tr("failed to open file to write") + ": " + tmpFile);
    in.stop();
    DOUT("exit: cannot write to file");
    return;
  }

  // - Pipe -
  BufferedStreamPipe pipe(&in, &out);
  connect(&pipe, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(this, SIGNAL(stopped()), &pipe, SLOT(stop()));

  if (!isStopped())
    pipe.run();
  out.close();

  bool flv;
  bool ok = isRunning() && pipe.isFinished() &&
      ((flv = FlvCodec::isFlvFile(tmpFile)) || Mp4Codec::isMp4File(tmpFile));
  if (ok) {
    suf = flv ? ".flv" : ".mp4";

    QString fileName = downloadPath() + FILE_PATH_SEP + name + suf;
    for (int i = 2; QFile::exists(fileName); i++)
      fileName = downloadPath() + FILE_PATH_SEP + name + " " + QString::number(i) + suf;
    //QFile::remove(fileName);
    ok =  QFile::rename(tmpFile, fileName);
    if (ok)
      setFileName(fileName);
    else
      emit error(tr("failed to rename downloaded file") + ": " + fileName);

    setState(Finished);
    qint64 size = in.size();
    emit progress(size, size);
    emit finished(this);
  } else {
    QFile::remove(tmpFile);
    if (!isStopped())
      setState(Error);;
    emit error(tr("download incomplete") + ": " + tmpFile);
  }
  DOUT("exit");
}

void
MrlDownloadTask::downloadMultipleMedia(const MediaInfo &mi, QNetworkCookieJar *jar)
{
  DOUT("enter");

  // - Start

  QString title = mi.title;
  if (title.isEmpty())
    title = tr("unknown");
  setTitle(title);

  Q_ASSERT(mi.mrls.size() >= 1);
  if (mi.mrls.isEmpty()) {
    setFileName(QString());
    setState(Error);
    DOUT("exit: empty mrl");
    return;
  }

  if (stopped_) {
    DOUT("exit: stopped");
    return;
  }

  // - Input

  QList<QUrl> urls;
  qint64 duration = 0;
  //qint64 size = 0;
  foreach (MrlInfo i, mi.mrls) {
    urls.append(i.url);
    duration += i.duration;
    //size += i.size;
  }

  QNetworkAccessManager *nam = new QNetworkAccessManager; // on the heap so that it is the last to delete
  if (jar)
    nam->setCookieJar(jar);
  InputStreamList ins;
  RemoteStream *first = 0;
  bool ok = false;
  foreach (const QUrl &url, urls) {
    ok = false;
    for (int i = 0; i < MaxDownloadRetries && !ok; i++) {
      RemoteStream *in = new BufferedRemoteStream(nam);
      if (!first) {
        first = in;
        in->request().setPriority(QNetworkRequest::NormalPriority);
      }
      connect(this, SIGNAL(stopped()), in, SLOT(stop()));

      QtExt::ProgressWithId *p = new QtExt::ProgressWithId((long)in, in);
      connect(in, SIGNAL(progress(qint64,qint64)), p, SLOT(trigger(qint64,qint64)));
      connect(p, SIGNAL(progress(qint64,qint64,long)), SLOT(updateProgressWithId(qint64,qint64,long)));

      in->setUrl(url);

      in->run();
      ins.append(in);

      if (stopped_) {
        stop();
        foreach (InputStream *s, ins)
          dynamic_cast<QObject *>(s)->deleteLater();
        nam->deleteLater();
        DOUT("exit: stopped");
        return;
      }

      in->waitForReady();

      QString contentType = in->contentType();
      ok = isMultiMediaMimeType(contentType);
      if (!ok) {
        DOUT("WARNING: invalid contentType =" << contentType << ", url =" << url.toString());
        ins.removeLast();
        p->deleteLater();
        in->deleteLater();
        if (first == in)
          first = 0;

        //emit warning(
        //  QString("%1 (%2/%3): ")
        //  .arg(tr("failed to fetch HTTP header, retry"))
        //  .arg(QString::number(i+1))
        //  .arg(QString::number(MaxDownloadRetries))
        //  + url.toString()
        //);
      }
    }
    if (!ok)
      break;
  }

  if (!ok) {
    emit error(tr("failed to get the first part") + ": " + urls.first().toString());
    emit stopped();
    foreach (InputStream *s, ins)
      dynamic_cast<QObject *>(s)->deleteLater();
    nam->deleteLater();

    if (retries_-- > 0 && !stopped_) {
      emit error(QString("%1 (%2/$3)")
        .arg(tr("retry"))
        .arg(QString::number(retries_--))
        .arg(QString::number(MaxDownloadRetries))
        + ": " + url()
      );
      QTimer::singleShot(0, this, SLOT(run())); // retry
    }
    DOUT("exit: retry");
    return;
  }

  Q_ASSERT(first);

  if (ins.isEmpty()) {
    setState(Error);
    emit error(tr("access denied to download URL") + ": " + first->url().toString());
    emit stopped();
    nam->deleteLater();
    return;
  }

  //if (!size)
  //  foreach (InputStream *in, ins_)
  //    size += in->size();

  // Output
  QString suf = ".flv";

  FileOutputStream out;
  QString name = QtExt::escapeFileName(title);
  QString tmpFile = downloadPath() + FILE_PATH_SEP "_" + name + suf;
  for (int i = 2; QFile::exists(tmpFile); i++)
    tmpFile = downloadPath() + FILE_PATH_SEP "_" + name + " " + QString::number(i) + suf;

  DOUT("fileName =" << tmpFile);
  setFileName(tmpFile);

  out.setFileName(tmpFile);
  if (!out.open() || stopped_) {
    setState(Error);
    emit error(tr("failed to open file to write") + ": " + tmpFile);
    emit stopped();
    foreach (InputStream *s, ins)
      dynamic_cast<QObject *>(s)->deleteLater();
    nam->deleteLater();
    DOUT("exit: cannot write to file");
    return;
  }

  // Merge

  FlvMerge merger; {
    merger.setInputStreams(ins);
    merger.setOutputStream(&out);
    merger.setDuration(duration);

    connect(&merger, SIGNAL(error(QString)), SIGNAL(error(QString)));
    connect(this, SIGNAL(stopped()), &merger, SLOT(stop()));
  }

  ins.reset();
  ok = merger.parse();
  if (!ok || stopped_) {
    setState(Error);
    emit error(tr("failed to parse FLV streams"));
    emit stopped();

   foreach (InputStream *s, ins)
      dynamic_cast<QObject *>(s)->deleteLater();
    nam->deleteLater();
    QFile::remove(fileName());
    return;
  }

  ins.reset();
  ok = merger.merge();
  if (ok)
    merger.finish();

  out.close();

  if (ok && isRunning() && FlvCodec::isFlvFile(tmpFile)) {
    ok = FlvCodec::updateFlvFileMeta(tmpFile);
    if (!ok)
      DOUT("warning: failed to update FLV meta:" << tmpFile);

    qint64 size = QFile(tmpFile).size();

    QString fileName = downloadPath() + FILE_PATH_SEP + name + suf;
    for (int i = 2; QFile::exists(fileName); i++)
      fileName = downloadPath() + FILE_PATH_SEP + name + " " + QString::number(i) + suf;
    //QFile::remove(fileName);
    ok =  QFile::rename(tmpFile, fileName);
    if (ok)
      setFileName(fileName);
    else
      emit error(tr("failed to rename downloaded file") + ": " + fileName);

    setState(Finished);
    emit progress(size, size);
    emit finished(this);
  } else {
    QFile::remove(tmpFile);
    if (!isStopped())
      setState(Error);;
    emit error(tr("download incomplete") + ": " + tmpFile);
  }

  emit stopped();

  foreach (InputStream *s, ins)
    dynamic_cast<QObject *>(s)->deleteLater();
  nam->deleteLater();
  DOUT("exit");
}

// - Progress -

void
MrlDownloadTask::updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id)
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

// EOF
