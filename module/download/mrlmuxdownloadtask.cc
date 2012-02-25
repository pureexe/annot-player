// mrlmuxdownloadtask.cc
// 2/22/2012
#include "mrlmuxdownloadtask.h"
#include "mrlmuxdownloadtaskprivate.h"
#include "module/datastream/bufferedremotestream.h"
#include "module/datastream/bufferedstreampipe.h"
#include "module/datastream/fileoutputstream.h"
#include "module/mediacodec/flvdemux.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/mp4codec.h"
#include "module/mp4box/mp4box.h"
#include "module/mrlresolver/mrlresolvermanager.h"
#include <QtNetwork>
#include <QtGui>

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
  connect(resolver_, SIGNAL(errorReceived(QString)), SIGNAL(error(QString)));
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
MrlMuxDownloadTask::suffixForUrl(const QString &url)
{
  if (url.contains(".nicovideo.jp/", Qt::CaseInsensitive))
    return "(nico H264 AAC).mp4";
  else if (url.contains(".bilibili.tv/", Qt::CaseInsensitive))
    return "(bili H264 AAC).mp4";
  else if (url.contains(".acfun.tv/", Qt::CaseInsensitive))
    return "(acfun H264 AAC).mp4";
  else if (url.contains(".youku.com/", Qt::CaseInsensitive))
    return "(youku H264 AAC).mp4";
  else if (url.contains(".sina.com.cn/", Qt::CaseInsensitive))
    return "(sina H264 AAC).mp4";
  else if (url.contains(".qq.com/", Qt::CaseInsensitive))
    return "(qq H264 AAC).mp4";
  else if (url.contains(".tudou.com/", Qt::CaseInsensitive))
    return "(tudou H264 AAC).mp4";
  else
    return "(H264 AAC).mp4";
}

QString
MrlMuxDownloadTask::escapeFileName(const QString &name)
{
  QString ret = name;
  ret.remove('"');
  ret.replace('/', "^");
  ret.replace('?', "H");
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
    setPath(desktopPath);
    setState(Error);
    DOUT("exit: empty mrl");
    return;
  }
  //QString realUrl = mi.mrls.first().url;
  //QString suf = realUrl.contains(QRegExp("[/\\.]mp4")) ? ".mp4" : ".flv";

  title = escapeFileName(title);
  QString baseName = desktopPath + "/_" + title;
  QString path = baseName + suffixForUrl(mi.mrls.first().url);

  InputStreamList ins;
  RemoteStream *first = 0;
  foreach (const MrlInfo &mrl, mi.mrls) {
    RemoteStream *in = new BufferedRemoteStream(this);
    if (!first)
      first = in;
    if (jar)
      in->networkAccessManager()->setCookieJar(jar);
    in->setRequest(QNetworkRequest(mrl.url));

    ProgressWithId *p = new ProgressWithId((long)in, this);
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

      path = baseName + ".mp4";
      downloadMedia(path);
      DOUT("exit");
      return;
    }
    first->reset();
    DOUT("download and remux to mp4");

    setPath(path);

    if (ins_.size() > 1)
      foreach (RemoteStream *in, ins_)
        if (in != first)
          in->run();

    QString videoPath = baseName + ".264";
    QString audioPath = baseName + ".aac";

    videoOut_->setFileName(videoPath);
    if (!videoOut_->open()) {
      stop();
      setState(Error);
      emit error(tr("failed to open file to write") + ": " + videoPath);
      return;
    }
    audioOut_->setFileName(audioPath);
    if (!audioOut_->open()) {
      stop();
      setState(Error);
      videoOut_->close();
      QFile::remove(videoPath);
      emit error(tr("failed to open file to write") + ": " + audioPath);
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
      QFile::remove(path);
      if (!isStopped())
        setState(Error);;
      emit error(tr("download incomplete") + ": " + path);
    }
    QFile::remove(audioPath);
    QFile::remove(videoPath);
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
  QString fileName = path();
  bool ret = Mp4Box::muxMp4File(fileName, tracks);
  if (!ret) {
    emit error(tr("failed to mux media") + ": " + fileName);
    DOUT("exit: failed to mux mp4");
    return false;
  }
  std::pair<int, int> dim = Mp4Codec::fileDimension(fileName);
  QString oldName = fileName;
  int i = fileName.lastIndexOf("/_");
#ifdef Q_OS_WIN
  if (i < 0)
    i = fileName.lastIndexOf("\\_");
#endif // Q_OS_WIN
  Q_ASSERT(i >= 0);
  fileName.remove(i + 1);
  if (dim.first && dim.second) {
    fileName.replace(QRegExp("H264 AAC\\)\\.mp4$"), QString("%1x%2 H264 AAC).mp4")
      .arg(QString::number(dim.first)).arg(QString::number(dim.second))
    );
  }
  QFile::remove(fileName);
  ret = QFile::rename(oldName, fileName);
  if (ret)
    setPath(fileName);
  else
    emit error(tr("failed to rename downloaded file") + ": " + fileName);
  DOUT("exit: ret =" << ret);
  return true; // always return true, even if renaming failed
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
MrlMuxDownloadTask::downloadMedia(const QString &path)
{
  DOUT("enter: path =" << path);
  Q_ASSERT(ins_.size() > 1);
  setPath(path);
  FileOutputStream *out = new FileOutputStream(path, this);
  if (!out->open()) {
    emit error(tr("failed to open file to write") + ": " + path);
    stop();
    setState(Error);
    DOUT("exit: cannot write to file");
    return;
  }

  RemoteStream *in = ins_.front();
  BufferedStreamPipe *pipe = new BufferedStreamPipe(in, out, this);
  //connect(pipe_, SIGNAL(finished()), SLOT(finish()));
  connect(pipe, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(this, SIGNAL(stopped()), pipe, SLOT(stop()));

  pipe->run();
  out->close();
  if (pipe->isFinished()) {
    // Rename file
    QString fileName = path;
    std::pair<int, int> dim = Mp4Codec::fileDimension(path);
    int i = fileName.lastIndexOf("/_");
#ifdef Q_OS_WIN
    if (i < 0)
      i = fileName.lastIndexOf("\\_");
#endif // Q_OS_WIN
    Q_ASSERT(i >= 0);
    fileName.remove(i + 1);
    if (dim.first && dim.second) {
      fileName.replace(QRegExp("\\.mp4$"), QString(" (%1x%2 H264 AAC).mp4")
        .arg(QString::number(dim.first)).arg(QString::number(dim.second))
      );
    }
    QFile::remove(fileName);
    bool ok = QFile::rename(path, fileName);
    if (ok)
      setPath(fileName);
    else
      emit error(tr("failed to rename downloaded file") + ": " + fileName);
    finish();
  } else {
    QFile::remove(path);
    if (!isStopped())
      setState(Error);;
    emit error(tr("download incomplete") + ": " + path);
  }
  DOUT("exit");
}

// EOF
