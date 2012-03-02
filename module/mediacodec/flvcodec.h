#ifndef FLVCODEC_H
#define FLVCODEC_H

// flvcodec.h
// 2/12/2012

#include <QObject>
#include <QList>

class InputStream;
class InputStreamList;
class OutputStream;

class Stoppable;
class MediaToc;

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

public:
  void demuxStream(InputStream *flv, OutputStream *vout, OutputStream *aout,
                   MediaToc *vtoc, MediaToc *atoc, bool async = true);
  void demuxStreamList(InputStreamList *flvs, const QList<qint64> &durations,
                       OutputStream *vout, OutputStream *aout,
                       MediaToc *vtoc, MediaToc *atoc, bool async = true);
  void demuxStreamList(InputStreamList *flvs,
                       OutputStream *vout, OutputStream *aout, bool async = true);
  void stop(); ///< Stop all background tasks.

  static bool isFlvFile(const QString &fileName);
  static bool isFlvStream(InputStream *flv);
  static int getFlvStreamDuration(InputStream *flv);
};

#endif // FLVCODEC_H
