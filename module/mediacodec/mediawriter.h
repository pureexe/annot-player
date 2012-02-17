#ifndef MEDIAWRITER_H
#define MEDIAWRITER_H

// mediawriter.h
// 2/12/2012
//
// See: FLVExtractCl/Library/FLVFile.cs
// http://moitah.net/

#include "module/datastream/outputstream.h"
#include <QObject>
#include <QByteArray>

class MediaToc;

class MediaWriter : public QObject
{
  Q_OBJECT
  typedef MediaWriter Self;
  typedef QObject Base;

public:
  explicit MediaWriter(QObject *parent = 0) : Base(parent) { }
  virtual ~MediaWriter() { }

  virtual QString codecName() const = 0;

  virtual void leadOut() { }
  virtual void writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader) = 0;
};

class DummyMediaWriter : public MediaWriter
{
  Q_OBJECT
  typedef DummyMediaWriter Self;
  typedef MediaWriter Base;
public:
  explicit DummyMediaWriter(QObject *parent = 0) : Base(parent) { }

  virtual QString codecName() const { return "Dummy"; } ///< \override

  virtual void writeFrame(const QByteArray &, quint32, bool) { } ///< \override
};

class MP3Writer : public DummyMediaWriter
{
  Q_OBJECT
  typedef DummyMediaWriter Base;
public:
  explicit MP3Writer(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "MP3"; } ///< \override
};
class SpeexWriter : public DummyMediaWriter
{
  Q_OBJECT
  typedef DummyMediaWriter Base;
public:
  explicit SpeexWriter(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "Speex"; } ///< \override
};
class WAVWriter : public DummyMediaWriter
{
  Q_OBJECT
  typedef DummyMediaWriter Base;
public:
  explicit WAVWriter(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "WAV"; } ///< \override
};
class AVIWriter : public DummyMediaWriter
{
  Q_OBJECT
  typedef DummyMediaWriter Base;
public:
  explicit AVIWriter(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "AVI"; } ///< \override
};

class AACWriter : public MediaWriter {
  Q_OBJECT
  typedef AACWriter Self;
  typedef MediaWriter Base;

  OutputStream *out_;
  MediaToc *toc_;
  int _aacProfile;
  int _sampleRateIndex;
  int _channelConfig;

public:
  AACWriter(OutputStream *out, MediaToc *toc, QObject *parent = 0)
    : Base(parent), out_(out), toc_(toc) { Q_ASSERT(out_); }

  virtual QString codecName() const { return "AAC"; } ///< \override

  virtual void writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader); ///< \override
  virtual void leadOut() { out_->finish(); } ///< \override

protected:
  void write(const char *data, int offset, int count)
  { out_->write(data + offset, count); }

  void write(const quint8 *data, int offset, int count)
  { write((const char *)data, offset, count); }
};


class RawH264Writer : public MediaWriter
{
  Q_OBJECT
  typedef RawH264Writer Self;
  typedef MediaWriter Base;

  OutputStream *out_;
  MediaToc *toc_;
  int _nalLengthSize;

public:
  RawH264Writer(OutputStream *out, MediaToc *toc, QObject *parent = 0)
    : Base(parent), out_(out), toc_(toc) { Q_ASSERT(out_); }

  virtual QString codecName() const { return "H264"; } ///< \override

  virtual void writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader); ///< \override
  virtual void leadOut() { out_->finish(); } ///< \override

protected:
  void write(const char *data, int offset, int count)
  { out_->write(data + offset, count); }

  void write(const quint8 *data, int offset, int count)
  { write((const char *)data, offset, count); }
};

#endif // MEDIAWRITER_H
