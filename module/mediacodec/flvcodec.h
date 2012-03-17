#ifndef FLVCODEC_H
#define FLVCODEC_H

// flvcodec.h
// 2/12/2012

#include <QObject>
#include <QList>
#include "module/stream/inputstream.h"

class InputStream;
class OutputStream;

class Stoppable;
class MediaToc;

struct FlvInfo
{
  qint64 duration; ///< in msec

  int height, width;

  int audioCodecId, videoCodecId;

  QString creator;

  FlvInfo()
    : duration(0), height(0), width(0), audioCodecId(0), videoCodecId(0) { }

  void clear()
  {
    duration = 0;
    width = height = 0;
    audioCodecId = videoCodecId = 0;
    creator.clear();
  }
};

class FlvCodec : public QObject
{
  Q_OBJECT
  typedef FlvCodec Self;
  typedef QObject Base;

  QList<Stoppable*> tasks_;

  // - Construction -
public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit FlvCodec(QObject *parent = 0) : Base(parent) { }
  ~FlvCodec() { stop(); }

public slots:
  void stop(); ///< Stop all background tasks.

public:
  void demuxStream(InputStream *flv, OutputStream *aout, OutputStream *vout,
                   MediaToc *atoc, MediaToc *vtoc, bool async = true);
  void demuxStream(InputStream *flv, OutputStream *aout, OutputStream *vout, bool async = true)
  { demuxStream(flv, aout, vout, 0, 0, async); }

  void demuxStreams(const InputStreamList &flvs, const QList<qint64> &durations,
                    OutputStream *aout, OutputStream *vout,
                    MediaToc *atoc, MediaToc *vtoc, bool async = true);
  void demuxStreams(const InputStreamList &flvs,
                    OutputStream *aout, OutputStream *vout, bool async = true);

  void mergeStream(const InputStreamList &ins,  OutputStream *out, bool async = true);

  FlvInfo analyzeStream(InputStream *in);
  FlvInfo analyzeStreams(const InputStreamList &ins);

public:
  static bool isFlvFile(const QString &fileName);
  static bool isFlvStream(InputStream *flv);
  static qint64 getLastTimestamp(InputStream *flv);
  static qint64 getLastTimestamp(const QByteArray &flv);

};

#endif // FLVCODEC_H
