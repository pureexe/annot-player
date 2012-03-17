#ifndef FLVPARSER_H
#define FLVPARSER_H

// flvparser.h
// 3/16/2012

#include "flvcodec.h"
#include "module/qtext/bitwise.h"
#include "module/qtext/stoppable.h"
#include "module/stream/inputstream.h"
#include <QObject>

// - Parserr -

class FlvParser : public QObject, public StoppableTask
{
  Q_OBJECT
  typedef FlvParser Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  InputStreamList ins_;

  FlvInfo meta_; // mostly from FLV script tag

  bool audioTagStripped_,
       videoTagStripped_,
       scriptTagStripped_;

public:
  explicit FlvParser(const InputStreamList &ins, QObject *parent = 0)
    : Base(parent), ins_(ins) { }

  explicit FlvParser(QObject *parent = 0)
    : Base(parent) { }

public:
  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

  const FlvInfo &meta() const { return meta_; }

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
  bool append(InputStream *in);
  bool readTag(InputStream *in);
  int scanScriptTag(QByteArray &data, int offset);

  void setMetaDouble(const QString &var, double value);
  void setMetaUInt8(const QString &var, quint8 value);
  void setMetaString(const QString &var, const QString &value);

protected:
  static bool read(InputStream *in, char *data, int offset, int count)
  { return count == in->read(data + offset, count); }

  static bool read(InputStream *in, quint8 *data, int offset, int count)
  { return read(in, (char *)data, offset, count); }

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
};

#endif // FLVPARSER_H
