// mrldownloadtask_s.cc
// Download multiple parts.
// 2/20/2012
#include "lib/downloadtask/mrldownloadtask.h"
#include "lib/qtext/filesystem.h"
#ifdef WITH_LIB_STREAM
# include "lib/stream/bufferedremotestream.h"
# include "lib/stream/bufferedstreampipe.h"
# include "lib/stream/fileoutputstream.h"
#else
# error "require lib stream"
#endif // WITH_LIB_STREAM
#ifdef WITH_LIB_MEDIACODEC
# include "lib/mediacodec/mp4codec.h"
# include "lib/mediacodec/flvcodec.h"
#else
# error "require lib mediacodec"
#endif // WITH_LIB_MEDIACODEC
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtNetwork/QNetworkAccessManager>

#define DEBUG "mrldownloadtask_s"
#include "lib/debug/debug.h"

enum { MaxDownloadRetries = 5 }; // TODO

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
  QNetworkAccessManager *nam = new QNetworkAccessManager; // on the heap so that it is the last to delete
  if (jar)
    nam->setCookieJar(jar);
  BufferedRemoteStream in_impl(nam);
  //nam->setParent(&in_impl);
  nam->deleteLater();

  RemoteStream &in = in_impl;
  connect(&in, SIGNAL(progress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
  connect(&in, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString)));
  connect(this, SIGNAL(stopped()), &in, SLOT(stop()));

  QString realUrl = mi.mrls.first().url;
  DOUT("realUrl =" << realUrl);

  in.setUrl(realUrl);
  in.run();
  in.waitForReady();

  if (stopped_) {
    stop();
    quit();
    DOUT("exit: stopped");
    return;
  }

  QString contentType = in.contentType();
  if (!isMultiMediaMimeType(contentType)) {
    setState(Error);
    emit errorMessage(tr("access denied to download URL") + ": " + realUrl);
    in.stop();
    quit();
    DOUT("exit: invalid file type");
    return;
  }

  // Output
  QString suf = contentType.contains("mp4", Qt::CaseInsensitive) ? ".mp4" : ".flv";

  FileOutputStream out;
  QString name = QtExt::escapeFileName(title);
  QString tmpFile = downloadPath() + QDir::separator() + "_" + name + suf;
  for (int i = 2; QFile::exists(tmpFile); i++)
    tmpFile = downloadPath() + QDir::separator() + "_" + name + " " + QString::number(i) + suf;

  DOUT("fileName =" << tmpFile);
  setFileName(tmpFile);

  out.setFileName(tmpFile);
  if (!out.open()) {
    setState(Error);
    emit errorMessage(tr("failed to open file to write") + ": " + tmpFile);
    in.stop();
    quit();
    DOUT("exit: cannot write to file");
    return;
  }

  // - Pipe -
  BufferedStreamPipe pipe(&in, &out);
  connect(&pipe, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString)));
  connect(this, SIGNAL(stopped()), &pipe, SLOT(stop()));

  if (!isStopped())
    pipe.run();
  out.close();

  bool flv;
  bool ok = isDownloading() && pipe.isFinished() &&
      QFileInfo(tmpFile).size() >= MinimumFileSize &&
      ((flv = FlvCodec::isFlvFile(tmpFile)) || Mp4Codec::isMp4File(tmpFile));
  if (ok) {
    suf = flv ? ".flv" : ".mp4";

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
    qint64 size = in.size();
    emit progress(size, size);
    emit finished(this);
  } else {
    QtExt::trashOrRemoveFile(tmpFile);
    if (!isStopped())
      setState(Error);;
    emit errorMessage(tr("download incomplete") + ": " + tmpFile);
  }
  quit();
  DOUT("exit");
}

// EOF
