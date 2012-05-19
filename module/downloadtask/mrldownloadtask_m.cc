// mrldownloadtask_m.cc
// Download single part.
// 2/20/2012
#include "module/downloadtask/mrldownloadtask.h"
#ifdef WITH_MODULE_STREAM
#  include "module/stream/bufferedremotestream.h"
#  include "module/stream/fileoutputstream.h"
#else
#  error "stream module is required"
#endif // WITH_MODULE_STREAM
#ifdef WITH_MODULE_MEDIACODEC
#  include "module/mediacodec/flvcodec.h"
#  include "module/mediacodec/flvmerge.h"
#else
#  error "mediacodec module is required"
#endif // WITH_MODULE_MEDIACODEC
#include "module/qtext/filesystem.h"
#include "module/qtext/network.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QTimer>

#define DEBUG "mrldownloadtask_m"
#include "module/debug/debug.h"

enum { MaxDownloadRetries = 5 };

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
