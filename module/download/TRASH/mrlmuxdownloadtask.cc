// mrlmuxdownloadtask.cc
// 2/22/2012
#include "mrlmuxdownloadtask.h"
#include "module/datastream/bufferedremotestream.h"
#include "module/datastream/bufferedstreampipe.h"
#include "module/datastream/fileoutputstream.h"
#include "module/mediacodec/flvdemux.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/mp4codec.h"
#include "module/mp4box/mp4box.h"
#include "module/mrlresolver/mrlresolvermanager.h"
#include "module/qtext/network.h"
#include <boost/tuple/tuple.hpp>
#include <QDesktopServices>
#include <QtNetwork>

#define DEBUG "mrlmuxdownloadtask"
#include "module/debug/debug.h"

// - Construction -

MrlMuxDownloadTask::MrlMuxDownloadTask(const QString &url, QObject *parent)
  : Base(url, parent), stopped_(false)
{
  videoOut_ = new FileOutputStream(this);
  audioOut_ = new FileOutputStream(this);

  demux_ = new FlvListDemux(this);
  connect(demux_, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(this, SIGNAL(stopped()), demux_, SLOT(stop()));

  resolver_ = new MrlResolverManager(this);
  connect(resolver_, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(resolver_, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)),
          SLOT(downloadAndMuxMedia(MediaInfo,QNetworkCookieJar*)));
}

// - Task -

void
MrlMuxDownloadTask::run()
{
  DOUT("enter");
  progresses_.clear();
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
MrlMuxDownloadTask::suffixForUrl(const QString &url, const QString &codecs)
{
  QString pad;
  if (!pad.isEmpty())
    pad.prepend(" ");
  if (url.contains(".nicovideo.jp/", Qt::CaseInsensitive))
    return " (nico" + pad + ").mp4";
  else if (url.contains(".bilibili.tv/", Qt::CaseInsensitive))
    return " (bili" + pad + ").mp4";
  else if (url.contains(".acfun.tv/", Qt::CaseInsensitive))
    return " (acfun" + pad + ").mp4";
  else if (url.contains(".youku.com/", Qt::CaseInsensitive))
    return " (youku" + pad + ").mp4";
  else if (url.contains(".sina.com.cn/", Qt::CaseInsensitive))
    return " (sina" + pad + ").mp4";
  else if (url.contains(".qq.com/", Qt::CaseInsensitive))
    return " (qq" + pad + ").mp4";
  else if (url.contains(".tudou.com/", Qt::CaseInsensitive))
    return " (tudou H264 AAC).mp4";
  else if (!codecs.isEmpty())
    return " (" + codecs + ").mp4";
  else
    return ".mp4";
}

QString
MrlMuxDownloadTask::escapeFileName(const QString &name)
{
  QString ret = name;
  ret.remove('"');
  ret.replace('/', "／");
  ret.replace(':', "-");
  ret.replace('?', "？");
  return ret.trimmed();
}

void
MrlMuxDownloadTask::downloadAndMuxMedia(const MediaInfo &mi, QNetworkCookieJar *jar)
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
  //QString realUrl = mi.mrls.first().url;
  //QString suf = realUrl.contains(QRegExp("[/\\.]mp4")) ? ".mp4" : ".flv";

  title = escapeFileName(title);
  QString baseName = desktopPath + "/_" + title;
  QString file = baseName + suffixForUrl(mi.mrls.first().url, "H264 AAC");

  InputStreamList ins;
  RemoteStream *first = 0;
  foreach (const MrlInfo &mrl, mi.mrls) {
    RemoteStream *in = new BufferedRemoteStream(this);
    if (!first)
      first = in;
    if (jar)
      in->networkAccessManager()->setCookieJar(jar);
    in->setRequest(QNetworkRequest(mrl.url));

    QtExt::ProgressWithId *p = new QtExt::ProgressWithId((long)in, this);
    connect(in, SIGNAL(progress(qint64,qint64)), p, SLOT(emit_progress(qint64,qint64)));
    connect(p, SIGNAL(progress(qint64,qint64,long)), SLOT(updateProgressWithId(qint64,qint64,long)));

    connect(in, SIGNAL(error(QString)), SIGNAL(error(QString)));
    connect(this, SIGNAL(stopped()), in, SLOT(stop()));
    ins_.append(in);
    ins.append(in);
  }
  Q_ASSERT(first);

  first->run();

  DOUT("eventloop enter");
  QEventLoop loop;
  connect(first, SIGNAL(error(QString)), &loop, SLOT(quit()));
  connect(first, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(first, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(this, SIGNAL(stopped()), &loop, SLOT(quit()));
  loop.exec();
  DOUT("eventloop leave");

  if (!isStopped()) {
    if (!FlvCodec::isFlvStream(first)) {
      first->reset();
      DOUT("not a flv stream, download as it is");
      //emit error(tr("failed to demux flv stream") + ": " + mi.mrls.first().url);

      file = baseName + ".mp4";
      downloadMedia(file);
      DOUT("exit");
      return;
    }
    first->reset();
    DOUT("download and remux to mp4");

    setFileName(file);

    if (ins_.size() > 1)
      foreach (RemoteStream *in, ins_)
        if (in != first)
          in->run();

    QString videoFile = baseName + ".264";
    QString audioFile = baseName + ".aac";

    videoOut_->setFileName(videoFile);
    if (!videoOut_->open()) {
      stop();
      setState(Error);
      emit error(tr("failed to open file to write") + ": " + videoFile);
      return;
    }
    audioOut_->setFileName(audioFile);
    if (!audioOut_->open()) {
      stop();
      setState(Error);
      videoOut_->close();
      QFile::remove(videoFile);
      emit error(tr("failed to open file to write") + ": " + audioFile);
      return;
    }

    demux_->setInputStreams(&ins);
    demux_->setVideoOut(videoOut_);
    demux_->setAudioOut(audioOut_);

    demux_->run();

    videoOut_->close();
    audioOut_->close();

    if (demux_->isFinished() && mux())
      finish();
    else {
      QFile::remove(file);
      if (!isStopped())
        setState(Error);;
      emit error(tr("download incomplete") + ": " + file);
    }
    QFile::remove(audioFile);
    QFile::remove(videoFile);
  }
  DOUT("exit");
}

void
MrlMuxDownloadTask::stop()
{
  DOUT("enter");
  stopped_ = true;
  setState(Stopped);
  emit stopped();
  DOUT("exit");
}

bool
MrlMuxDownloadTask::mux()
{
  DOUT("enter");
  QStringList tracks;
  tracks.append(videoOut_->fileName());
  tracks.append(audioOut_->fileName());
  QString file = fileName();
  Mp4Box::Mp4Info info;
  bool ret = Mp4Box::muxMp4File(file, tracks, &info);
  if (ret)
    ret = Mp4Codec::isMp4File(file);
  if (!ret) {
    emit error(tr("failed to mux media") + ": " + file);
    DOUT("exit: failed to mux mp4");
    return false;
  }
  int width = info.width,
      height = info.height;
  if (!width || !height)
    boost::tie(width, height) = Mp4Codec::fileDimension(file);
  QString oldFile = file;
#ifdef Q_OS_WIN
  file.replace("\\", "/");
#endif // Q_OS_WIN
  int i = file.lastIndexOf("/_");
  if (i >= 0)
    file.remove(i + 1, 1);
  if (width && height) {
    file.replace(QRegExp("H264 AAC\\)\\.mp4$"), QString("%1x%2 H264 AAC).mp4")
      .arg(QString::number(width)).arg(QString::number(height))
    );
  }
  if (file != oldFile) {
    QFile::remove(file);
    ret = QFile::rename(oldFile, file);
    if (ret)
      setFileName(file);
    else
      emit error(tr("failed to rename downloaded file") + ": " + file);
  }
  DOUT("exit: ret =" << ret);
  return ret; // always return true, even if renaming failed
}

void
MrlMuxDownloadTask::finish()
{
  DOUT("enter");
  setState(Finished);

  qint64 totalBytes = 0;
  foreach (const Progress &p, progresses_)
    totalBytes += p.totalBytes;

  emit progress(totalBytes, totalBytes);
  emit finished(this);
  DOUT("exit");
}

// - Progress -

void
MrlMuxDownloadTask::updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id)
{
  Progress &p = progresses_[id];
  p.receivedBytes = receivedBytes;
  p.totalBytes = totalBytes;

  receivedBytes = 0;
  totalBytes = 0;
  foreach (const Progress &v, progresses_) {
    receivedBytes += v.receivedBytes;
    totalBytes += v.totalBytes;
  }
  emit progress(receivedBytes, totalBytes);
}

void
MrlMuxDownloadTask::downloadMedia(const QString &file)
{
  DOUT("enter: file =" << file);
  Q_ASSERT(ins_.size() > 1);
  setFileName(file);
  FileOutputStream *out = new FileOutputStream(file, this);
  if (!out->open()) {
    emit error(tr("failed to open file to write") + ": " + file);
    stop();
    setState(Error);
    DOUT("exit: cannot write to file");
    return;
  }

  RemoteStream *in = ins_.first();
  BufferedStreamPipe *pipe = new BufferedStreamPipe(in, out, this);
  //connect(pipe_, SIGNAL(finished()), SLOT(finish()));
  connect(pipe, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(this, SIGNAL(stopped()), pipe, SLOT(stop()));

  pipe->run();
  out->close();
  bool mp4;
  if (pipe->isFinished() &&
     ((mp4 = Mp4Codec::isMp4File(file)) || FlvCodec::isFlvFile(file))
     ) {
    // Rename file
    QString newFile = file;
#ifdef Q_OS_WIN
    newFile.replace("\\", "/");
#endif // Q_OS_WIN
    int i = newFile.lastIndexOf("/_");
    if (i >= 0)
      newFile.remove(i + 1, 1);
    if (!mp4)
      newFile.replace(QRegExp("\\.mp4$"), ".flv");
    if (newFile != file) {
      QFile::remove(newFile);
      bool ok = QFile::rename(file, newFile);
      if (ok)
        setFileName(newFile);
      else
        emit error(tr("failed to rename downloaded file") + ": " + newFile);
    }
    finish();
  } else {
    QFile::remove(fileName());
    if (!isStopped())
      setState(Error);;
    emit error(tr("download incomplete") + ": " + fileName());
  }
  DOUT("exit");
}

// EOF
