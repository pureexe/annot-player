#ifndef FLVMERGE_H
#define FLVMERGE_H

// flvmerge.h
// 3/14/2012

#include "module/qtext/bitwise.h"
#include "module/stream/inputstream.h"
#include "module/stream/outputstream.h"
#include "module/stream/streampipe.h"
#include <QObject>

// - Merger -

class FlvMerge : public QObject, public StreamListPipe
{
  Q_OBJECT
  typedef FlvMerge Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputStreamList ins_;
  OutputStream *out_;
  qint64 audioTimestamp_, videoTimestamp_,
         lastAudioTimestamp_, lastVideoTimestamp_,
         lastAudioTimestep_, lastVideoTimestep_;
  qint64 duration_;

  bool audioTagStripped_,
       videoTagStripped_,
       scriptTagStripped_;

public:
  FlvMerge(const InputStreamList &ins, OutputStream *out, QObject *parent = 0)
    : Base(parent), ins_(ins), out_(out),
      audioTimestamp_(0), videoTimestamp_(0), duration_(0) { }

  explicit FlvMerge(const InputStreamList &ins, QObject *parent = 0)
    : Base(parent), ins_(ins), out_(0),
      audioTimestamp_(0), videoTimestamp_(0), duration_(0) { }

  explicit FlvMerge(QObject *parent = 0)
    : Base(parent), out_(0),
      audioTimestamp_(0), videoTimestamp_(0), duration_(0) { }

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  void setInputStreams(const InputStreamList &ins) { ins_ = ins; } ///< \override
  void setOutputStream(OutputStream *out) { out_ = out; } ///< \override
  void setDuration(qint64 msec) { duration_ = msec; }

  qint64 duration() const { return duration_; }

  qint64 timestamp() const { return qMax(audioTimestamp_, videoTimestamp_); }

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

  void leadOut();

public:
  bool parse();
  bool merge();

protected:
  InputStreamList inputStreams() const { return ins_; } ///< \override
  OutputStream *outputStream()  const { return out_; } ///< \override

  bool append(InputStream *in, bool writeHeader);
  bool readTag(InputStream *in, bool writeHeader);
  int updateScriptTag(QByteArray &data, int offset) const;
private:
  bool updateScriptTagDoubleValue(quint8 *data, const QString &var) const;
  bool updateScriptTagUInt8Value(quint8 *data, const QString &var) const;

protected:
  static bool read(InputStream *in, char *data, int offset, int count)
  { return count == in->read(data + offset, count); }

  static bool read(InputStream *in, quint8 *data, int offset, int count)
  { return read(in, (char *)data, offset, count); }

  static bool write(OutputStream *out, const char *data, int offset, int count)
  { return count == out->write(data + offset, count); }

  static bool write(OutputStream *out, const quint8 *data, int offset, int count)
  { return write(out, (const char *)data, offset, count); }

  static quint8 readUInt8(InputStream *in, bool *ok)
  { quint8 byte; *ok = read(in, &byte, 0, 1); return byte; }

  static quint16 readUInt16(InputStream *in, bool *ok)
  { quint8 x[2]; *ok = read(in, x, 0, 2); return Bitwise::BigEndian::toUInt16(x); }

  static quint32 readUInt24(InputStream *in, bool *ok)
  { quint8 x[4]; x[0] = 0; *ok = read(in, x, 1, 3); return Bitwise::BigEndian::toUInt32(x); }

  static quint32 readUInt32(InputStream *in, bool *ok)
  { quint8 x[4]; *ok = read(in, x, 0, 4); return Bitwise::BigEndian::toUInt32(x); }

  static quint64 readUInt64(InputStream *in, bool *ok)
  { quint8 x[8]; *ok = read(in, x, 0, 8); return Bitwise::BigEndian::toUInt64(x); }

  static double readDouble(InputStream *in, bool *ok)
  { quint8 x[sizeof(double)]; *ok = read(in, x, 0, sizeof(double)); return Bitwise::BigEndian::toDouble(x); }

  static bool writeUInt8(OutputStream *out, quint8 value)
  { return write(out, &value, 0, 1); }

  static bool writeUInt16(OutputStream *out, quint16 value)
  { quint8 x[2]; Bitwise::BigEndian::getBytes(x, value); return write(out, x, 0, 2); }

  static bool writeUInt24(OutputStream *out, quint32 value)
  { quint8 x[4]; Bitwise::BigEndian::getBytes(x, value); return write(out, x, 1, 3); }

  static bool writeUInt32(OutputStream *out, quint32 value)
  { quint8 x[4]; Bitwise::BigEndian::getBytes(x, value); return write(out, x, 0, 4); }
};

#endif // FLVMERGE_H
