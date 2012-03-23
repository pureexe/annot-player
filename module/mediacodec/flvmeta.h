#ifndef FLVMETA_H
#define FLVMETA_H

// flvmeta.h
// 3/16/2012

#include "module/qtext/stoppable.h"
#include "module/stream/inputstream.h"
#include "module/stream/outputstream.h"
#include "module/stream/inputoutputstream.h"
#include <QObject>
#include <cstring>

// - FLV Script tag -

struct FlvMeta
{
  qint64 duration; ///< in msec
  qint64 lastTimestamp, lastKeyFrameTimestamp;
  qint64 lastKeyFrameLocation;
  qint64 fileSize, audioSize, videoSize, dataSize;

  int height, width;

  int audioCodecId, videoCodecId;

  double audioDataRate, videoDataRate, frameRate;

  //bool hasKeyFrames, hasMetaData, canSeekToEnd;

  QString creator, metaDataCreator;

  FlvMeta()
    : duration(0), lastTimestamp(0), lastKeyFrameTimestamp(0), lastKeyFrameLocation(0),
      fileSize(0), audioSize(0), videoSize(0), dataSize(0),
      height(0), width(0), audioCodecId(0), videoCodecId(0),
      audioDataRate(0), videoDataRate(0), frameRate(0) { }
      //hasKeyFrames(true), hasMetaData(true), canSeekToEnd(true) { }

  void clear()
  {
    duration = lastTimestamp = lastKeyFrameTimestamp = lastKeyFrameLocation =
    fileSize = audioSize = videoSize = dataSize = 0;
    height = width = audioCodecId = videoCodecId = 0;
    audioDataRate = videoDataRate = frameRate = 0;
    //hasKeyFrames = hasMetaData = canSeekToEnd = true;
    creator.clear();
    metaDataCreator.clear();
  }
};


// - Parser -

class FlvMetaReader : public QObject, public StoppableTask
{
  Q_OBJECT
  typedef FlvMetaReader Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputStreamList ins_;

  FlvMeta meta_; // mostly from FLV script tag

  bool audioTagStripped_,
       videoTagStripped_,
       scriptTagStripped_;

public:
  explicit FlvMetaReader(const InputStreamList &ins, QObject *parent = 0)
    : Base(parent), state_(Stopped), ins_(ins) { }

  explicit FlvMetaReader(QObject *parent = 0)
    : Base(parent), state_(Stopped) { }

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  const FlvMeta &meta() const { return meta_; }

  void setInputStreams(const InputStreamList &ins) { ins_ = ins; }

signals:
  void stopped();
  //void timestampChanged(qint64);
  void error(QString message);

public slots:
  virtual void run(); ///< \override

  virtual void stop() ///< \override
  {
    state_ = Stopped;
    emit stopped();
  }

  bool parse();

public:
  bool parseStream(InputStream *in)
  {
    Q_ASSERT(in);
    InputStreamList ins;
    ins.append(in);
    return parseStreams(ins);
  }

  bool parseStreams(const InputStreamList &ins)
  {
    setInputStreams(ins);
    return parse();
  }

protected:
  bool readStream(InputStream *in);
  static bool seekToScriptTag(InputStream *in);
  bool readScriptTag(InputStream *in);

  void setMetaDouble(const QString &var, double value);
  void setMetaUInt8(const QString &var, quint8 value);
  void setMetaString(const QString &var, const QString &value);
};

// - Writer -

class FlvMetaWriter : public QObject, public StoppableTask
{
  Q_OBJECT
  typedef FlvMetaWriter Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputOutputStream *ios_;

  FlvMeta meta_;

public:
  explicit FlvMetaWriter(const FlvMeta &meta, InputOutputStream *ios = 0, QObject *parent = 0)
    : Base(parent), state_(Stopped), ios_(ios), meta_(meta) { }
  explicit FlvMetaWriter(InputOutputStream *ios, QObject *parent = 0)
    : Base(parent), state_(Stopped), ios_(ios) { }

  explicit FlvMetaWriter(QObject *parent = 0)
    : Base(parent), state_(Stopped), ios_(0) { }

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  void setMeta(const FlvMeta &meta) { meta_ = meta; }
  const FlvMeta &meta() const { return meta_; }

  void setStream(InputOutputStream *ios) { ios_ = ios; }

signals:
  void stopped();
  //void timestampChanged(qint64);
  void error(QString message);

public slots:
  virtual void run(); ///< \override

  virtual void stop() ///< \override
  {
    state_ = Stopped;
    emit stopped();
  }

  bool update();

public:
  bool updateStream(InputOutputStream *ios)
  {
    Q_ASSERT(ios);
    setStream(ios);
    return update();
  }

protected:
  static bool seekToScriptTag(InputStream *in);
  bool updateScriptTag();
  void finish();

  bool updateScriptTagDoubleValue(OutputStream *out, const QString &var) const;
  bool updateScriptTagUInt8Value(OutputStream *out, const QString &var) const;
};

// - Creator -

class FlvMetaCreator : public QObject, public StoppableTask
{
  Q_OBJECT
  typedef FlvMetaCreator Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  FlvMeta meta_;

  InputStream *in_;

public:
  explicit FlvMetaCreator(InputStream *in, QObject *parent = 0)
    : Base(parent), state_(Stopped), in_(in) { }

  explicit FlvMetaCreator(QObject *parent = 0)
    : Base(parent), state_(Stopped), in_(0) { }

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  void setInputStream(InputStream *in) { in_ = in; }

  const FlvMeta &meta() const { return meta_; }

signals:
  void stopped();
  //void timestampChanged(qint64);
  void error(QString message);

public slots:
  virtual void run(); ///< \override

  virtual void stop() ///< \override
  {
    state_ = Stopped;
    emit stopped();
  }

  bool parse();
public:
  bool parseStream(InputStream *in)
  {
    Q_ASSERT(in);
    setInputStream(in);
    return parse();
  }

protected:
  bool readTag();
};

#endif // FLVMETA_H
