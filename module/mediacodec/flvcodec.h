#ifndef FLVCODEC_H
#define FLVCODEC_H

// flvcodec.h
// 2/12/2012

#include "module/mediacodec/flvmeta.h"
#include "module/stream/inputstream.h"
#include <QtCore/QObject>
#include <QtCore/QList>

class InputStream;
class OutputStream;
class InputOutputStream;

class Stoppable;
class MediaToc;

class FlvCodec : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(FlvCodec)
  typedef FlvCodec Self;
  typedef QObject Base;

  QList<Stoppable *> tasks_;

  // - Construction -
public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit FlvCodec(QObject *parent = nullptr) : Base(parent) { }
  ~FlvCodec() { stop(); }

public slots:
  void stop(); ///< Stop all background tasks.

public:
  void demuxFlvStream(InputStream *flv, OutputStream *aout, OutputStream *vout,
                   MediaToc *atoc, MediaToc *vtoc, bool async = true);
  void demuxFlvStream(InputStream *flv, OutputStream *aout, OutputStream *vout, bool async = true)
  { demuxFlvStream(flv, aout, vout, 0, 0, async); }

  void demuxFlvStreams(const InputStreamList &flvs, const QList<qint64> &durations,
                    OutputStream *aout, OutputStream *vout,
                    MediaToc *atoc, MediaToc *vtoc, bool async = true);
  void demuxFlvStreams(const InputStreamList &flvs,
                    OutputStream *aout, OutputStream *vout, bool async = true);

  void mergeFlvStreams(const InputStreamList &ins,  OutputStream *out, bool async = true);
  static bool mergeFlvFiles(const QStringList &ins, const QString &out); // async = false

  FlvMeta analyzeFlvStream(InputStream *in);
  FlvMeta analyzeFlvStreams(const InputStreamList &ins);

public:
  static bool isFlvFile(const QString &fileName);
  static bool isFlvStream(InputStream *flv);
  static qint64 getLastTimestamp(InputStream *flv);
  static qint64 getLastTimestamp(const QByteArray &flv);

  static bool updateFlvFileMeta(const QString &fileName);
  static bool updateFlvStreamMeta(InputOutputStream *ios);

};

#endif // FLVCODEC_H
