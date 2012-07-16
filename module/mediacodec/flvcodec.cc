// flvcodec.cc
// 2/12/2012
// See: FLVExtractCl/Library/FLVFile.cs
// http://moitah.net/

#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/flvdemux.h"
#include "module/mediacodec/flvmerge.h"
#include "module/mediacodec/flvmeta.h"
#include "module/stream/fileinputstream.h"
#include "module/stream/fileoutputstream.h"
#include "module/stream/filestream.h"
#include "module/stream/inputstream.h"
#include "module/stream/outputstream.h"
#include "module/qtext/stoppable.h"
#include <QtCore/QStringList>
#include <QtCore/QThreadPool>

#define DEBUG "flvcodec"
#include "module/debug/debug.h"

// - Task -

void
FlvCodec::demuxFlvStream(InputStream *flv, OutputStream *aout, OutputStream *vout,
                         MediaToc *atoc, MediaToc *vtoc, bool async)
{
  DOUT("enter: async =" << async);
  Q_ASSERT(flv);
  flv->reset();
  FlvDemux *t = new FlvDemux(flv, this);
  t->setAutoDelete(false);
  t->setAudioOut(aout);
  t->setVideoOut(vout);
  t->setAudioToc(atoc);
  t->setVideoToc(vtoc);
  tasks_.append(t);
  if (async)
    QThreadPool::globalInstance()->start(t);
  else
    t->run();
  DOUT("exit");
}

void
FlvCodec::demuxFlvStreams(const InputStreamList &flvs, const QList<qint64> &durations,
                          OutputStream *aout, OutputStream *vout,
                          MediaToc *atoc, MediaToc *vtoc, bool async)
{
  DOUT("enter: async =" << async);
  Q_ASSERT(!flvs.isEmpty());
  flvs.reset();
  FlvListDemux *t = new FlvListDemux(flvs, durations, this);
  t->setAutoDelete(false);
  t->setAudioOut(aout);
  t->setVideoOut(vout);
  t->setAudioToc(atoc);
  t->setVideoToc(vtoc);
  tasks_.append(t);
  if (async)
    QThreadPool::globalInstance()->start(t);
  else
    t->run();
  DOUT("exit");
}

void
FlvCodec::demuxFlvStreams(const InputStreamList &flvs,
                          OutputStream *aout, OutputStream *vout, bool async)
{
  DOUT("enter: async =" << async);
  Q_ASSERT(!flvs.isEmpty());
  flvs.reset();
  FlvListDemux *t = new FlvListDemux(this);
  t->setAutoDelete(false);
  t->setInputStreams(flvs);
  t->setAudioOut(aout);
  t->setVideoOut(vout);
  tasks_.append(t);
  if (async)
    QThreadPool::globalInstance()->start(t);
  else
    t->run();
  DOUT("exit");
}

void
FlvCodec::stop()
{
  if (!tasks_.isEmpty())
    foreach (Stoppable *t, tasks_)
      t->stop();
}

// - Helper -

// See: http://www.jqueryphp.com/how-to-get-flv-file-duration/2009/08/
bool
FlvCodec::isFlvStream(InputStream *input)
{
  Q_ASSERT(input);
  input->reset();
  //if (input->size() < 3)
  //  return false;
  input->reset();
  char data[3];
  int count = input->read(data, sizeof(data));
  return count == 3 &&
      data[0] == 'F' &&
      data[1] == 'L' &&
      data[2] == 'V';
}

bool
FlvCodec::isFlvFile(const QString &fileName)
{
  FileInputStream fin(fileName);
  return isFlvStream(&fin);
}

// See: http://www.jqueryphp.com/how-to-get-flv-file-duration/2009/08/
qint64
FlvCodec::getLastTimestamp(InputStream *input)
{
  Q_ASSERT(input);
  qint64 size = input->size();
  qint64 pos = size - 3;
  if (pos < 0)
    return 0;
  if (!input->seek(pos))
    return 0;
  char data[4];
  data[0] = 0;
  int count = input->read(data + 1, 3);
  if (count != 3)
    return 0;
  pos = size - (qint64)Bitwise::BigEndian::toUInt32((quint8*)data);
  if (pos < 0 || pos >= size)
    return 0;
  if (!input->seek(pos))
    return 0;
  count = input->read(data + 1, 3);
  if (count != 3)
    return 0;
  return Bitwise::BigEndian::toUInt32((quint8*)data);
}

qint64
FlvCodec::getLastTimestamp(const QByteArray &input)
{
  qint64 size = input.size();
  qint64 pos = size - 3;
  if (pos < 0)
    return 0;
  char data[4];
  data[0] = 0;
  qMemCopy(data + 1, input.data() + pos, 3);
  pos = size - (qint64)Bitwise::BigEndian::toUInt32((quint8*)data);
  if (pos < 0 || pos >= size)
    return 0;
  qMemCopy(data + 1, input.data() + pos, 3);
  return Bitwise::BigEndian::toUInt32((quint8*)data);
}

void
FlvCodec::mergeFlvStreams(const InputStreamList &ins, OutputStream *out, bool async)
{
  DOUT("enter: async =" << async);
  Q_ASSERT(!ins.isEmpty());
  Q_ASSERT(out);
  ins.reset();
  FlvMerge *t = new FlvMerge(ins, out, this);
  t->setAutoDelete(false);
  tasks_.append(t);
  if (async)
    QThreadPool::globalInstance()->start(t);
  else
    t->run();
  DOUT("exit");
}

bool
FlvCodec::mergeFlvFiles(const QStringList &input, const QString &output)
{
  if (input.isEmpty())
    return false;
  DOUT("enter");

  FileOutputStream fout(output);
  if (!fout.open()) {
    DOUT("exit: failed to open output:" << output);
    return false;
  }

  InputStreamList ins;
  foreach (const QString &fileName, input)
    ins.append(new FileInputStream(fileName));
  globalInstance()->mergeFlvStreams(ins, &fout, false); // async = false
  fout.finish();
  foreach (InputStream *p, ins)
    delete p;
  return QFile::exists(output);
}


FlvMeta
FlvCodec::analyzeFlvStream(InputStream *in)
{
  DOUT("enter");
  Q_ASSERT(in);
  FlvMetaReader t;
  t.setAutoDelete(false);
  //tasks_.append(&t);

  t.parseStream(in);
  //tasks_.removeAll(&t);
  DOUT("exit");
  return t.meta();
}

FlvMeta
FlvCodec::analyzeFlvStreams(const InputStreamList &ins)
{
  DOUT("enter: stream.count =" << ins.size());
  FlvMetaReader t;
  t.setAutoDelete(false);
  //tasks_.append(&t);

  t.parseStreams(ins);
  //tasks_.removeAll(&t);
  DOUT("exit");
  return t.meta();
}

bool
FlvCodec::updateFlvFileMeta(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  FileStream fs(fileName);
  if (!fs.isOpen()) {
    DOUT("exit: failed to open file");
    return false;
  }
  bool ok = updateFlvStreamMeta(&fs);
  DOUT("exit: ret =" << ok);
  return ok;
}

bool
FlvCodec::updateFlvStreamMeta(InputOutputStream *ios)
{
  DOUT("enter");
  Q_ASSERT(ios);
  FlvMetaCreator creator;
  ios->reset();
  bool ok = creator.parseStream(ios);
  if (!ok) {
    DOUT("exit: failed to parse meta from FLV file");
    return false;
  }

  FlvMeta meta = creator.meta();
  FlvMetaWriter writer(meta);
  ios->reset();
  ok = writer.updateStream(ios);
  if (!ok) {
    DOUT("exit: failed to update meta in FLV file");
    return false;
  }

  DOUT("exit: ret =" << ok);
  return ok;
}

// EOF
