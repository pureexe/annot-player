// mrldownloadtask.cc
// 2/20/2012
#include "mrldownloadtask.h"
#include "module/datastream/bufferedremotestream.h"
#include "module/datastream/fileoutputstream.h"
#include "module/datastream/bufferedstreampipe.h"
#include "module/mrlresolver/mrlresolvermanager.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/mp4codec.h"
#include <QDesktopServices>
#include <QtNetwork>

#define DEBUG "mrldownloadtask"
#include "module/debug/debug.h"

// - Construction -

MrlDownloadTask::MrlDownloadTask(const QString &url, QObject *parent)
  : Base(url, parent), stopped_(false)
{
  in_ = new BufferedRemoteStream(this);
  connect(in_, SIGNAL(progress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
  connect(in_, SIGNAL(error(QString)), SIGNAL(error(QString)));

  out_ = new FileOutputStream(this);

  pipe_ = new BufferedStreamPipe(in_, out_, this);
  //connect(pipe_, SIGNAL(finished()), SLOT(finish()));
  connect(pipe_, SIGNAL(error(QString)), SIGNAL(error(QString)));

  // Stop pipe_ before in_;
  connect(this, SIGNAL(stopped()), pipe_, SLOT(stop()));
  connect(this, SIGNAL(stopped()), in_, SLOT(stop()));

  resolver_ = new MrlResolverManager(this);
  connect(resolver_, SIGNAL(errorReceived(QString)), SIGNAL(error(QString)));
  connect(resolver_, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)),
          SLOT(downloadMedia(MediaInfo,QNetworkCookieJar*)));
}

// - Task -

void
MrlDownloadTask::run()
{
  DOUT("enter");
  setState(Downloading);
  bool ok = resolver_->resolveMedia(url());
  if (!ok) {
    setState(Error);
    emit error(tr("failed to download from URL") + ": " + url());
  }
  DOUT("exit");
}

// - Download -

QString
MrlDownloadTask::escapeFileName(const QString &name)
{
  QString ret = name;
  ret.remove('"');
  ret.replace('/', "／");
  ret.replace(':', "-");
  ret.replace('?', "？");
  return ret.trimmed();
}

void
MrlDownloadTask::downloadMedia(const MediaInfo &mi, QNetworkCookieJar *jar)
{
  DOUT("enter");
  setState(Downloading);
  QString title = mi.title;
  if (title.isEmpty())
    title = tr("unknown");
  setTitle(title);

  QString desktopPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);

  if (mi.mrls.isEmpty()) {
    setFileName(QString());
    setState(Error);
    DOUT("exit: empty mrl");
    return;
  }

  if (jar)
    in_->networkAccessManager()->setCookieJar(jar);

  QString realUrl = mi.mrls.first().url;
  QString suf = realUrl.contains(QRegExp("[/\\.]mp4")) ? ".mp4" : ".flv";

  title = escapeFileName(title);
  QString tmpFile = desktopPath + "/_" + title + suf;
  setFileName(tmpFile);

  DOUT("realUrl =" << realUrl);
  DOUT("fileName =" << tmpFile);

  out_->setFileName(tmpFile);
  if (!out_->open()) {
    setState(Error);
    emit error(tr("failed to open file to write") + ": " + tmpFile);
    DOUT("exit: cannot write to file");
    return;
  }

  in_->setRequest(QNetworkRequest(realUrl));
  in_->run();

  DOUT("eventloop enter");
  QEventLoop loop;
  connect(in_, SIGNAL(error(QString)), &loop, SLOT(quit()));
  connect(in_, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(in_, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(this, SIGNAL(stopped()), &loop, SLOT(quit()));
  loop.exec();
  DOUT("eventloop leave");

  if (!isStopped())
    pipe_->run();
  out_->close();
  bool flv;
  if (pipe_->isFinished() &&
      ((flv = FlvCodec::isFlvFile(tmpFile)) || Mp4Codec::isMp4File(tmpFile))) {
    suf = flv ? "flv" : ".mp4";
    QString file = desktopPath + "/" + title + suf;
    QFile::remove(file);
    if (QFile::rename(tmpFile, file))
      setFileName(file);
    else
      emit error(tr("failed to rename downloaded file") + ": " + file);
    finish();
  } else {
    QFile::remove(tmpFile);
    if (!isStopped())
      setState(Error);;
    emit error(tr("download incomplete") + ": " + tmpFile);
  }
  DOUT("exit");
}

void
MrlDownloadTask::stop()
{
  stopped_ = true;
  setState(Stopped);
  emit stopped();
}

void
MrlDownloadTask::finish()
{
  DOUT("enter");
  setState(Finished);
  int size = in_->size();
  emit progress(size, size);
  emit finished(this);
  DOUT("exit");
}

// EOF
