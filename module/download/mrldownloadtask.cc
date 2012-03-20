// mrldownloadtask.cc
// 2/20/2012
#include "mrldownloadtask.h"
#include "module/mrlresolver/mrlresolvermanager.h"
#include "module/stream/bufferedremotestream.h"
#include "module/stream/bufferedstreampipe.h"
#include "module/stream/fileoutputstream.h"
#include "module/mediacodec/mp4codec.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/flvmerge.h"
#include "module/qtext/filesystem.h"
#include "module/qtext/network.h"
#include <QDesktopServices>
#include <QtNetwork>

#define DEBUG "mrldownloadtask"
#include "module/debug/debug.h"

// - Helper -

bool
MrlDownloadTask::isMultiMediaMimeType(const QString &contentType)
{ return !contentType.isEmpty() && !contentType.startsWith("text/", Qt::CaseInsensitive); }

// - Construction -

MrlDownloadTask::MrlDownloadTask(const QString &url, QObject *parent)
  : Base(url, parent), stopped_(false)
{
  resolver_ = new MrlResolverManager(this);
  connect(resolver_, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(resolver_, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)),
          SLOT(downloadMedia(MediaInfo,QNetworkCookieJar*)));
}

// - Task -

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
  setState(Downloading);
  progress_.clear();
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

  QString title = mi.title;
  if (title.isEmpty())
    title = tr("unknown");
  setTitle(title);

  switch (mi.mrls.size()) {
  case 0: emit error(tr("failed to resolve media URL") +": " + mi.refurl); break;
  case 1: downloadSingleMedia(mi, jar); break;
  default: downloadMultipleMedia(mi, jar); break;
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
  setState(Downloading);

  // - Input
  BufferedRemoteStream in_impl;
  RemoteStream &in = in_impl;
  connect(&in, SIGNAL(progress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
  connect(&in, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(this, SIGNAL(stopped()), &in, SLOT(stop()));
  if (jar)
    in.networkAccessManager()->setCookieJar(jar);

  QString realUrl = mi.mrls.first().url;
  DOUT("realUrl =" << realUrl);

  in.setUrl(realUrl);
  in.run();
  in.waitForReady();

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
  QString desktopPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString name = QtExt::escapeFileName(title);
  QString tmpFile = desktopPath + "/_" + name + suf;

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
  BufferedStreamPipe pipe(&in, &out, this);
  connect(&pipe, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(this, SIGNAL(stopped()), &pipe, SLOT(stop()));

  if (!isStopped())
    pipe.run();
  out.close();

  bool flv;
  bool ok = pipe.isFinished() &&
      (flv = FlvCodec::isFlvFile(tmpFile)) || Mp4Codec::isMp4File(tmpFile);
  if (ok) {
    suf = flv ? ".flv" : ".mp4";

    QString fileName = desktopPath + "/" + name + suf;
    QFile::remove(fileName);
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
  setState(Downloading);

  // - Input

  QList<QUrl> urls;
  qint64 duration = 0;
  //qint64 size = 0;
  foreach (MrlInfo i, mi.mrls) {
    urls.append(i.url);
    duration += i.duration;
    //size += i.size;
  }

  InputStreamList ins;
  RemoteStream *first = 0;
  foreach (const QUrl &url, urls) {
    RemoteStream *in = new BufferedRemoteStream(this);
    if (!first)
      first = in;
    connect(this, SIGNAL(stopped()), in, SLOT(stop()));

    QtExt::ProgressWithId *p = new QtExt::ProgressWithId((long)in, this);
    connect(in, SIGNAL(progress(qint64,qint64)), p, SLOT(emit_progress(qint64,qint64)));
    connect(p, SIGNAL(progress(qint64,qint64,long)), SLOT(updateProgressWithId(qint64,qint64,long)));

    in->setUrl(url);

    if (jar)
      in->networkAccessManager()->setCookieJar(jar);
    in->run();
    in->waitForReady();
    ins.append(in);
  }
  //if (jar)
  //  jar->setParent(this);

  Q_ASSERT(first);

  QString contentType = first->contentType();
  if (!isMultiMediaMimeType(contentType)) {
    setState(Error);
    emit error(tr("access denied to download URL") + ": " + first->url().toString());
    emit stopped();
    return;
  }

  //if (!size)
  //  foreach (InputStream *in, ins_)
  //    size += in->size();

  // Output
  QString suf = contentType.contains("mp4", Qt::CaseInsensitive) ? ".mp4" : ".flv";

  FileOutputStream out;
  QString desktopPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString name = QtExt::escapeFileName(title);
  QString tmpFile = desktopPath + "/_" + name + suf;

  DOUT("fileName =" << tmpFile);
  setFileName(tmpFile);

  out.setFileName(tmpFile);
  if (!out.open()) {
    setState(Error);
    emit error(tr("failed to open file to write") + ": " + tmpFile);
    emit stopped();
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
  bool ok = merger.parse();
  if (!ok) {
    setState(Error);
    emit error(tr("failed to parse FLV streams"));
    emit stopped();
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

    QString fileName = desktopPath + "/" + name + suf;
    QFile::remove(fileName);
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
