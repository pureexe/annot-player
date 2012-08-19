// mrldownloadtask_m.cc
// Download multiple parts.
// 2/20/2012
//
// QNetworkAccessManager is not shared, because of 6 requests limits, as follows:
// Note: http://qt-project.org/doc/qt-4.8/QNetworkAccessManager.html
// QNetworkAccessManager queues the requests it receives. The number of requests executed in parallel is dependent on the protocol. Currently, for the HTTP protocol on desktop platforms, 6 requests are executed in parallel for one host/port combination.

#include "module/downloadtask/mrldownloadtask.h"
#ifdef WITH_MODULE_STREAM
# include "module/stream/bufferedremotestream.h"
# include "module/stream/fileoutputstream.h"
#else
# error "stream module is required"
#endif // WITH_MODULE_STREAM
#ifdef WITH_MODULE_MEDIACODEC
# include "module/mediacodec/flvcodec.h"
# include "module/mediacodec/flvmerge.h"
#else
# error "mediacodec module is required"
#endif // WITH_MODULE_MEDIACODEC
#ifdef WITH_MODULE_MRLRESOLVER
# include "module/mrlresolver/luamrlresolver.h"
#else
# error "mrlresolver module is required"
#endif // WITH_MODULE_MRLRESOLVER
#include "module/qtext/network.h"
#include "module/qtext/filesystem.h"
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkCookieJar>

#define DEBUG "mrldownloadtask_m"
#include "module/debug/debug.h"

enum { MaxDownloadRetries = 10, MinDownloadRetries = 2 };
enum { MaxIndividualDownloadRetries = 5 };

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
    quit();
    DOUT("exit: empty mrl");
    return;
  }

  if (stopped_) {
    quit();
    DOUT("exit: stopped");
    return;
  }

  // - Input

  QList<QUrl> urls;
  qint64 duration = 0;
  //qint64 size = 0;
  foreach (const MrlInfo &i, mi.mrls) {
    urls.append(i.url);
    duration += i.duration;
    //size += i.size;
  }

  //QNetworkAccessManager *nam = new QNetworkAccessManager; // on the heap so that it is the last to delete
  //if (jar)
  //  nam->setCookieJar(jar);
  InputStreamList ins;
  //RemoteStream *first = 0;
  bool ok = false;
  int count = 0;
  QNetworkAccessManager *nam = nullptr;
  int namCount = 0;
  DOUT("urls size =" << urls.size());
  for (int j = 0; j < MaxDownloadRetries && !ok && !stopped_; j++) {
    DOUT("j =" << j);
    if (j && !stopped_) {
      DOUT("download failed");
      count--;
      emit errorMessage(
        QString("%1 (%2/%3): ")
        .arg(tr("failed to fetch part %1, retry").arg(QString::number(count+1)))
        .arg(QString::number(j+1))
        .arg(QString::number(MaxDownloadRetries))
        + url()
      );

      if (j >= MinDownloadRetries) {
        enum { WaitInterval = 5 }; // wait 5 seconds
        int secs = WaitInterval + j - MinDownloadRetries;
        emit errorMessage(tr("wait %1 seconds and try again").arg(QString::number(secs)) + " ...");
        DOUT(QString("wait for %1 seconds").arg(QString::number(secs)));
        sleep_.start(secs);
      }
      if (stopped_) {
        DOUT("stopped, break");
        break;
      }

      DOUT("with mrlresolver");
      LuaMrlResolver resolver_impl;
      MrlResolver &resolver = resolver_impl;
      resolver.setSynchronized(true);
      //connect(resolver, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SLOT(updateUrls(MediaInfo,QNetworkCookieJar*)));
      resolver.resolveMedia(url());
      const MediaInfo &mi = resolver.resolvedMediaInfo();
      QNetworkCookieJar *jar = resolver.resolvedCookieJar();
      if (jar)
        jar->setParent(nullptr); // memory leak
      nam = nullptr; // reset nam

      urls.clear();
      foreach (const MrlInfo &i, mi.mrls)
        urls.append(i.url);
    }
    ok = true;
    for (; count < urls.size() && ok && !stopped_; count++) {
      DOUT("count =" << count);
      const QUrl url = urls[count];
      //if (stopped_) {
      //  stop();
      //  foreach (InputStream *in, QtExt::revertList(ins)) { // revert list so that nam will be deleted later
      //    RemoteStream *p = dynamic_cast<RemoteStream *>(in);
      //    p->stop();
      //    p->deleteLater();
      //  }
      //  //nam->deleteLater();
      //  if (jar)
      //    jar->deleteLater();
      //  quit();
      //  DOUT("exit: stopped");
      //  return;
      //}
      ok = false;
      for (int i = 0; i < MaxIndividualDownloadRetries && !ok && !stopped_; i++) {
        RemoteStream *in = new BufferedRemoteStream(nullptr);
        if (!nam || count % QtExt::MaxConcurrentNetworkRequestCount == namCount) {
          if (!nam)
            namCount = count % QtExt::MaxConcurrentNetworkRequestCount;
          nam = new QNetworkAccessManager(in);
          if (jar) {
            nam->setCookieJar(jar);
            jar->setParent(nullptr);
          }
        }
        Q_ASSERT(nam);
        in->setNetworkAccessManager(nam);
        connect(this, SIGNAL(stopped()), in, SLOT(stop()));

        QtExt::ProgressWithId *p = new QtExt::ProgressWithId((long)in, in);
        connect(in, SIGNAL(progress(qint64,qint64)), p, SLOT(trigger(qint64,qint64)));
        connect(p, SIGNAL(progress(qint64,qint64,long)), SLOT(updateProgressWithId(qint64,qint64,long)));

        in->setUrl(url);
        //if (!first) {
        //  first = in;
        //  //in->request().setPriority(QNetworkRequest::NormalPriority);
        //}

        DOUT("count =" << count << ", i =" << i);
        in->run();
        ins.append(in);

        if (stopped_)
          break;
        in->waitForReady();
        QString contentType = in->contentType();
        ok = isMultiMediaMimeType(contentType);
        if (!ok) {
          DOUT("WARNING: invalid contentType =" << contentType << ", url =" << url.toString());
          ins.removeLast();
          //if (first == in)
          //  first = 0;
          //p->deleteLater();
          //in->deleteLater();
          in->stop();
          //in->deleteLater();
          if (nam && nam->parent() == in)
            nam = nullptr;
          DOUT("deleting current stream");
          delete in;
          DOUT("current stream deleted");

          //emit warning(
          //  QString("%1 (%2/%3): ")
          //  .arg(tr("failed to fetch HTTP header, retry"))
          //  .arg(QString::number(i+1))
          //  .arg(QString::number(MaxDownloadRetries))
          //  + url.toString()
          //);
        }
      }
    }
    DOUT("access HTTP header ok =" << ok);
  }

   if (!ok || isStopped()) {
     DOUT("ins size =" << ins.size());
     if (!ins.isEmpty()) {
       auto p = ins.constEnd(); // revert list so that nam will be deleted later
       while (p != ins.constBegin()) { // foreach (InputStream *in, QtExt::revertList(ins)) {
         DOUT("deleting input stream");
         auto in = static_cast<RemoteStream *>(*--p);
         in->stop();
         DOUT("deleting previous stream");
         //in->deleteLater();
         delete in;
         DOUT("previous stream deleted");
       }
       ins.clear();
     }
     if (jar) {
       DOUT("deleting cookie jar");
       //jar->deleteLater();
       delete jar;
      DOUT("cookie jar deleted");
     }
   }
  if (ins.isEmpty()) {
    setState(Error);
    emit errorMessage(tr("access denied to download URL") + ": " + url());
    emit stopped();
    if (jar) {
      DOUT("deleting cookie jar");
      //jar->deleteLater();
      delete jar;
      DOUT("cookie jar deleted");
    }
    //nam->deleteLater();
    quit();
    DOUT("exit: download failure");
    return;
  }

  //if (!size)
  //  foreach (InputStream *in, ins_)
  //    size += in->size();

  // Output
  QString suf = ".flv";

  FileOutputStream out;
  QString name = QtExt::escapeFileName(title);
  QString tmpFile = downloadPath() + QDir::separator() + "_" + name + suf;
  for (int i = 2; QFile::exists(tmpFile); i++)
    tmpFile = downloadPath() + QDir::separator() + "_" + name + " " + QString::number(i) + suf;

  DOUT("fileName =" << tmpFile);
  setFileName(tmpFile);

  out.setFileName(tmpFile);
  if (!out.open() || stopped_) {
    setState(Error);
    emit errorMessage(tr("failed to open file to write") + ": " + tmpFile);
    emit stopped();
    auto p = ins.constEnd(); // revert list so that nam will be deleted later
    while (p != ins.constBegin()) { // foreach (InputStream *in, QtExt::revertList(ins)) {
      auto in = static_cast<RemoteStream *>(*--p);
      in->stop();
      DOUT("deleting previous stream");
      //in->deleteLater();
      delete in;
      DOUT("previous stream delete");
    }
    if (jar) {
      DOUT("deleting cookie jar");
      //jar->deleteLater();
      delete jar;
      DOUT("cookie jar deleted");
    }
    //nam->deleteLater();
    quit();
    DOUT("exit: cannot write to file");
    return;
  }

  // Merge

  FlvMerge merger; {
    merger.setInputStreams(ins);
    merger.setOutputStream(&out);
    merger.setDuration(duration);

    connect(&merger, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString)));
    connect(this, SIGNAL(stopped()), &merger, SLOT(stop()));
  }

  ins.reset();
  ok = merger.parse();
  if (!ok || stopped_) {
    setState(Error);
    emit errorMessage(tr("failed to parse FLV streams"));
    emit stopped();

    auto p = ins.constEnd(); // revert list so that nam will be deleted later
    while (p != ins.constBegin()) { // foreach (InputStream *in, QtExt::revertList(ins))
      auto in = static_cast<RemoteStream *>(*--p);
      in->stop();
      DOUT("deleting previous stream");
      //in->deleteLater();
      delete in;
      DOUT("previous stream deleted");
    }
    if (jar) {
      DOUT("deleting cookie jar");
      //jar->deleteLater();
      delete jar;
      DOUT("cookie jar deleted");
    }
    //nam->deleteLater();
    QtExt::trashOrRemoveFile(fileName());
    quit();
    return;
  }

  ins.reset();
  ok = merger.merge();
  if (ok)
    merger.finish();

  out.close();

  if (ok && isDownloading() &&
      QFileInfo(tmpFile).size() >= MinimumFileSize &&
      FlvCodec::isFlvFile(tmpFile)) {
    ok = FlvCodec::updateFlvFileMeta(tmpFile);
    if (!ok)
      DOUT("warning: failed to update FLV meta:" << tmpFile);

    qint64 size = QFile(tmpFile).size();

    QString fileName = downloadPath() + QDir::separator() + name + suf;
    //for (int i = 2; QFile::exists(fileName); i++)
    //  fileName = downloadPath() + QDir::separator() + name + " " + QString::number(i) + suf;
    QtExt::trashOrRemoveFile(fileName);
    ok =  QFile::rename(tmpFile, fileName);
    if (ok)
      setFileName(fileName);
    else
      emit errorMessage(tr("failed to rename downloaded file") + ": " + fileName);

    setState(Finished);
    emit progress(size, size);
    emit finished(this);
  } else {
    QtExt::trashOrRemoveFile(tmpFile);
    if (!isStopped())
      setState(Error);;
    emit errorMessage(tr("download incomplete") + ": " + tmpFile);
  }

  emit stopped();

  auto p = ins.constEnd(); // revert list so that nam will be deleted later
  while (p != ins.constBegin()) { // foreach (InputStream *in, QtExt::revertList(ins))
    auto in = static_cast<RemoteStream *>(*--p);
    in->stop();
    DOUT("deleting previous stream");
    //in->deleteLater();
    delete in;
    DOUT("previous stream deleted");
  }
  if (jar) {
    DOUT("deleting cookie jar");
    //jar->deleteLater();
    delete jar;
    DOUT("cookie jar deleted");
  }
  quit();
  //nam->deleteLater();
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
