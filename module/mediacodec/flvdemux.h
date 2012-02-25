#ifndef FLVDEMUX_H
#define FLVDEMUX_H

// flvdemux.h
// 2/12/2012
//
// See: FLVExtractCl/Library/FLVFile.cs

// http://moitah.net/

#include <QObject>
#include <QRunnable>
#include "module/qtext/bitwise.h"
#include "module/qtext/stoppable.h"
#include "module/datastream/inputstream.h"
#include "module/datastream/outputstream.h"

// - Demuxer -

class MediaToc;
class MediaWriter;

class FlvDemux : public QObject, public QRunnable, public Stoppable
{
  Q_OBJECT
  typedef FlvDemux Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputStream *inputStream_;
  OutputStream *videoOutputStream_,
               *audioOutputStream_;
  MediaToc *videoToc_,
           *audioToc_;

  MediaWriter *videoWriter_,
              *audioWriter_;
  bool writeHeader_;

  //FractionUInt32 _averageFrameRate,
  //               _trueFrameRate;
  //QList<quint32> _videoTimeStamps;

public:
  explicit FlvDemux(InputStream *in, QObject *parent = 0)
    : Base(parent), state_(Stopped),
      inputStream_(in), videoOutputStream_(0), audioOutputStream_(0),
      videoToc_(0), audioToc_(0),
      videoWriter_(0), audioWriter_(0), writeHeader_(true) { }

  explicit FlvDemux(QObject *parent = 0)
    : Base(parent), state_(Stopped),
      inputStream_(0), videoOutputStream_(0), audioOutputStream_(0),
      videoToc_(0), audioToc_(0),
      videoWriter_(0), audioWriter_(0), writeHeader_(true) {  }

  ~FlvDemux();

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  void setInput(InputStream *in) { inputStream_ = in; }
  void setVideoOut(OutputStream *out) { videoOutputStream_ = out; }
  void setAudioOut(OutputStream *out) { audioOutputStream_ = out; }

  void setVideoToc(MediaToc *toc) { videoToc_ = toc; }
  void setAudioToc(MediaToc *toc) { audioToc_ = toc; }

  OutputStream *videoOut() const { return videoOutputStream_; }
  OutputStream *audioOut() const { return audioOutputStream_; }

  MediaToc *videoToc() const { return videoToc_; }
  MediaToc *audioToc() const { return audioToc_; }

signals:
  void stopped();
  void timestampChanged(qint64);
  void error(QString message);

public slots:
  virtual void run() ///< \override
  {
    if (demux())
      leadOut();
  }

  virtual void stop() ///< \override
  {
    state_ = Stopped;
    emit stopped();
  }

  void leadOut();

  void setWriteHeader(bool t) { writeHeader_ = t; }
public:
  bool demux();

protected:
  MediaWriter *makeAudioWriter(quint32 mediaInfo);
  MediaWriter *makeVideoWriter(quint32 mediaInfo);

protected:
  bool readTag();
  //FractionUInt32 calculateAverageFrameRate() const;
  //FractionUInt32 calculateTrueFrameRate() const;

protected:
  void seek(qint64 offset) { inputStream_->seek(offset); }
  qint64 pos() const { return inputStream_->pos(); }

  bool read(char *data, int offset, int count)
  { return count == inputStream_->read(data + offset, count); }

  bool read(quint8 *data, int offset, int count)
  { return read((char *)data, offset, count); }

  quint32 readUInt8(bool *ok)
  { quint8 byte; *ok = read(&byte, 0, 1); return byte; }

  quint32 readUInt24(bool *ok)
  { quint8 x[4]; x[0] = 0; *ok = read(x, 1, 3); return Bitwise::BigEndian::toUInt32(x, 0); }

  quint32 readUInt32(bool *ok)
  { quint8 x[4]; *ok = read(x, 0, 4); return Bitwise::BigEndian::toUInt32(x, 0); }
};

class FlvListDemux : public QObject, public QRunnable, public Stoppable
{
  Q_OBJECT
  typedef FlvListDemux Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputStreamList *ins_;
  QList<qint64> durations_;

  int streamIndex_; // current stream index
  qint64 duration_; // current duration

  FlvDemux *demux_;

  //FractionUInt32 _averageFrameRate,
  //               _trueFrameRate;
  //QList<quint32> _videoTimeStamps;

public:
  FlvListDemux(InputStreamList *ins, const QList<qint64> &durations, QObject *parent = 0)
    : Base(parent), state_(Stopped), ins_(ins), durations_(durations) { init(); }

  explicit FlvListDemux(InputStreamList *ins, QObject *parent = 0)
    : Base(parent), state_(Stopped), ins_(ins) { init(); }

  explicit FlvListDemux(QObject *parent = 0)
    : Base(parent), state_(Stopped), ins_(0) { init(); }

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  void setInputStreams(InputStreamList *ins) { ins_ = ins; }
  void setDurations(const QList<qint64> &durations) { durations_ = durations; }

  void setVideoOut(OutputStream *out) { demux_->setVideoOut(out); }
  void setAudioOut(OutputStream *out) { demux_->setAudioOut(out); }

  void setVideoToc(MediaToc *toc) { demux_->setVideoToc(toc); }
  void setAudioToc(MediaToc *toc) { demux_->setAudioToc(toc); }

private:
  void init()
  {
    demux_ = new FlvDemux(this);
    connect(demux_, SIGNAL(timestampChanged(qint64)), SLOT(updateTimestamp(qint64)));
  }

signals:
  void stopped();
  void timestampChanged(qint64);
  void error(QString message);

public slots:
  virtual void run() ///< \override
  {
    if (demux())
      demux_->leadOut();
  }

  virtual void stop() ///< \override
  {
    state_ = Stopped;
    demux_->stop();
    emit stopped();
  }

protected:
  bool demux();

protected slots:
  void updateTimestamp(qint64 ts) { emit timestampChanged(duration_ + ts); }
};


#endif // FLVDEMUX_H
