// flvcodec.cc
// 2/12/2012
// See: FLVExtractCl/Library/FLVFile.cs
// http://moitah.net/

#include "flvcodec.h"
#include "flvdemux.h"
#include "module/datastream/inputstream.h"
#include "module/datastream/outputstream.h"
#include "module/qtext/bitwise.h"
#include "module/qtext/stoppable.h"
#include <QtCore>

#define DEBUG "flvcodec"
#include "module/debug/debug.h"

// - Task -

void
FLVCodec::demuxStream(InputStream *flv, OutputStream *vout, OutputStream *aout,
                      MediaToc *vtoc, MediaToc *atoc, bool async)
{
  DOUT("enter: async =" << async);
  Q_ASSERT(flv);
  flv->reset();
  FLVDemux *t = new FLVDemux(flv, this);
  t->setAutoDelete(false);
  t->setVideoOut(vout);
  t->setAudioOut(aout);
  t->setVideoToc(vtoc);
  t->setAudioToc(atoc);
  if (async)
    QThreadPool::globalInstance()->start(t);
  else
    t->run();
  DOUT("exit");
}

void
FLVCodec::demuxStreamList(InputStreamList *flvs, const QList<qint64> &durations,
                          OutputStream *vout, OutputStream *aout,
                          MediaToc *vtoc, MediaToc *atoc, bool async)
{
  DOUT("enter: async =" << async);
  Q_ASSERT(flvs);
  flvs->reset();
  FLVListDemux *t = new FLVListDemux(flvs, durations, this);
  t->setAutoDelete(false);
  t->setVideoOut(vout);
  t->setAudioOut(aout);
  t->setVideoToc(vtoc);
  t->setAudioToc(atoc);
  if (async)
    QThreadPool::globalInstance()->start(t);
  else
    t->run();
  DOUT("exit");
}

void
FLVCodec::stop()
{
  if (!tasks_.isEmpty())
    foreach (Stoppable *t, tasks_)
      t->stop();
}

// - Helper -

// See: http://www.jqueryphp.com/how-to-get-flv-file-duration/2009/08/
bool
FLVCodec::isFLVStream(InputStream *input)
{
  Q_ASSERT(input);
  input->reset();
  if (input->size() < 3)
    return false;
  input->reset();
  char data[3];
  int count = input->read(data, sizeof(data));
  return count == 3 &&
      data[0] == 'F' &&
      data[1] == 'L' &&
      data[2] == 'V';
}

// See: http://www.jqueryphp.com/how-to-get-flv-file-duration/2009/08/
int
FLVCodec::getFLVStreamDuration(InputStream *input)
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
  data[0] = 0;
  count = input->read(data + 1, 3);
  if (count != 3)
    return 0;
  return Bitwise::BigEndian::toUInt32((quint8*)data);
}

// EOF
