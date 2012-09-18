#ifndef FLVDEMUX_H
#define FLVDEMUX_H

// flvdemux.h
// 2/12/2012
//
// See: FLVExtractCl/Library/FLVFile.cs
// http://moitah.net/

#include "qtx/qxstoppable.h"
#include "lib/stream/inputstream.h"
#include "lib/stream/outputstream.h"
#include <QtCore/QObject>

// - Demuxer -

class MediaToc;
class MediaWriter;

class FlvDemux : public QObject, public QxStoppableTask
{
  Q_OBJECT
  Q_DISABLE_COPY(FlvDemux)
  typedef FlvDemux Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputStream *in_;
  OutputStream *audioOut_,
               *videoOut_;
  MediaToc *audioToc_,
           *videoToc_;

  MediaWriter *audioWriter_,
              *videoWriter_;
  bool writeHeader_;

  //FractionUInt32 _averageFrameRate,
  //               _trueFrameRate;
  //QList<quint32> _videoTimeStamps;

public:
  explicit FlvDemux(InputStream *in, QObject *parent = nullptr)
    : Base(parent), state_(Stopped),
      in_(in), audioOut_(nullptr), videoOut_(nullptr),
      audioToc_(nullptr), videoToc_(nullptr),
      audioWriter_(nullptr), videoWriter_(nullptr), writeHeader_(true) { }

  explicit FlvDemux(QObject *parent = nullptr)
    : Base(parent), state_(Stopped),
      in_(nullptr), audioOut_(nullptr), videoOut_(nullptr),
      audioToc_(nullptr), videoToc_(nullptr),
      audioWriter_(nullptr), videoWriter_(nullptr), writeHeader_(true) {  }

  ~FlvDemux();

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  void setInput(InputStream *in) { in_ = in; }
  void setAudioOut(OutputStream *out) { audioOut_ = out; }
  void setVideoOut(OutputStream *out) { videoOut_ = out; }

  void setAudioToc(MediaToc *toc) { audioToc_ = toc; }
  void setVideoToc(MediaToc *toc) { videoToc_ = toc; }

  OutputStream *audioOut() const { return audioOut_; }
  OutputStream *videoOut() const { return videoOut_; }

  MediaToc *audioToc() const { return audioToc_; }
  MediaToc *videoToc() const { return videoToc_; }

signals:
  void stopped();
  //void timestampChanged(qint64);
  void errorMessage(QString message);

public slots:
  void run() override;

  void stop() override
  {
    state_ = Stopped;
    emit stopped();
  }

  void finish();

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
};

class FlvListDemux : public QObject, public QxStoppableTask
{
  Q_OBJECT
  Q_DISABLE_COPY(FlvListDemux)
  typedef FlvListDemux Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputStreamList ins_;
  QList<qint64> durations_;

  int streamIndex_; // current stream index
  qint64 duration_; // current duration

  FlvDemux *demux_;

  //FractionUInt32 _averageFrameRate,
  //               _trueFrameRate;
  //QList<quint32> _videoTimeStamps;

public:
  FlvListDemux(const InputStreamList &ins, const QList<qint64> &durations, QObject *parent = nullptr)
    : Base(parent), state_(Stopped), ins_(ins), durations_(durations) { init(); }

  explicit FlvListDemux(const InputStreamList &ins, QObject *parent = nullptr)
    : Base(parent), state_(Stopped), ins_(ins) { init(); }

  explicit FlvListDemux(QObject *parent = nullptr)
    : Base(parent), state_(Stopped) { init(); }

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  void setInputStreams(const InputStreamList &ins) { ins_ = ins; }
  void setDurations(const QList<qint64> &durations) { durations_ = durations; }

  void setAudioOut(OutputStream *out) { demux_->setAudioOut(out); }
  void setVideoOut(OutputStream *out) { demux_->setVideoOut(out); }

  void setAudioToc(MediaToc *toc) { demux_->setAudioToc(toc); }
  void setVideoToc(MediaToc *toc) { demux_->setVideoToc(toc); }

private:
  void init()
  {
    demux_ = new FlvDemux(this);
    //connect(demux_, SIGNAL(timestampChanged(qint64)), SLOT(updateTimestamp(qint64)));
  }

signals:
  void stopped();
  //void timestampChanged(qint64);
  void errorMessage(QString message);

public slots:
  void run() override;

  void stop() override
  {
    state_ = Stopped;
    demux_->stop();
    emit stopped();
  }

protected:
  bool demux();

protected slots:
  //void updateTimestamp(qint64 ts) { emit timestampChanged(duration_ + ts); }
};


#endif // FLVDEMUX_H
