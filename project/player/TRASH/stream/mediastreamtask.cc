// mediastreamtask.cc
// 2/16/2016

#include "mediastreamtask.h"
#include "module/streamservice/streamservice.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/mediatoc.h"
//#include "module/mp4box/mp4box.h"
#include "module/mediacodec/codecinfo.h"
#include "module/datastream/bufferedremotestream.h"
#include "module/datastream/bufferedfifostream.h"
#include <QtCore>
#include <QtNetwork>

//#define DEBUG "mediastreamtask"
#include "module/debug/debug.h"

#ifdef DEBUG
  #include "module/datastream/fileinputstream.h"
  #include "module/mediacodec/flvcodec.h"
#endif // DEBUG

void
MediaStreamTask::stop()
{
  if (nam_) delete nam_; nam_ = 0;
  if (ins_) delete ins_; ins_ = 0;
}

void
MediaStreamTask::run()
{
#ifdef DEBUG
  debug();
  return;
#endif // DEBUG

  if (msi_.urls.isEmpty())
    return;

  if (!nam_)
    nam_ = new QNetworkAccessManager;

  // http://www.bilibili.tv/video/av214176/
  ins_ = new InputStreamList;

  qint64 totalDuration = 0;
  qint64 totalSize = 0;
  RemoteStream *first = 0;
  for (int i = 0; i < msi_.urls.size(); i++) {
    RemoteStream *in = new BufferedRemoteStream(nam_);
    QString url = msi_.urls[i];
    if (url.isEmpty())
      continue;
    in->setUrl(url);
    if (i == 0)
      first = in;
    if (msi_.durations.size() >= i) {
      qint64 duration = msi_.durations[i];
      in->setSize(duration);
      totalDuration += duration;
    }
    if (msi_.sizes.size() >= i) {
      qint64 size = msi_.sizes[i];
      in->setSize(size);
      totalSize += size;
    }
  }

  Q_ASSERT(first);
  if (!first)
    return;
  {
    first->run();
    QEventLoop loop;
    connect(first, SIGNAL(readyRead()), &loop, SLOT(quit()));
    connect(first, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
  }

  FifoStream *vout = new BufferedFifoStream;
  FifoStream *aout = new BufferedFifoStream;
  MediaToc *vtoc = new MediaToc;
  MediaToc *atoc = new MediaToc;

  vout->setSize(totalSize); // estimated max siz
  aout->setSize(totalSize); // estimated max size

  FlvCodec::globalInstance()->demuxStreamList(ins_, msi_.durations, vout, aout, vtoc, atoc);

  StreamService *ss = StreamService::globalInstance();
  if (ss->isActive())
    ss->stop();
  ss->clear();
  ss->addStream(MT_H264, vout, vtoc);
  ss->addStream(MT_AAC, aout, atoc);
  ss->setDuration(totalDuration);

  ss->start();
}

void
MediaStreamTask::debug()
{
#ifdef DEBUG
  if (msi_.urls.isEmpty())
    return;

  if (!nam_)
    nam_ = new QNetworkAccessManager;

  // http://www.bilibili.tv/video/av214176/
  // http://www.bilibili.tv/video/av214156/
  ins_ = new InputStreamList;

  InputStream *in = new FileInputStream("/Users/jichi/tmp/bilibili.flv");

  FifoStream *vout = new BufferedFifoStream;
  FifoStream *aout = new BufferedFifoStream;
  MediaToc *vtoc = new MediaToc;
  MediaToc *atoc = new MediaToc;

  vout->setSize(in->size()); // estimated max siz
  aout->setSize(in->size()); // estimated max size

  qint64 duration = FlvCodec::getFlvStreamDuration(in);
  FlvCodec::globalInstance()->demuxStream(in, vout, aout, vtoc, atoc);

  StreamService *ss = StreamService::globalInstance();
  if (ss->isActive())
    ss->stop();
  ss->clear();
  ss->addStream(MT_H264, vout, vtoc);
  ss->addStream(MT_AAC, aout, atoc);
  ss->setDuration(duration);

  ss->start();
#endif // DEBUG
}

// EOF
