#ifndef MEDIAWRITER_H
#define MEDIAWRITER_H

// mediawriter.h
// 2/12/2012
//
// See: FLVExtractCl/Library/FLVFile.cs
// http://moitah.net/

#include "module/stream/outputstream.h"
#include <QtCore/QByteArray>
#include <QtCore/QObject>

class MediaToc;

class MediaWriter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MediaWriter)
  typedef MediaWriter Self;
  typedef QObject Base;

public:
  explicit MediaWriter(QObject *parent = 0) : Base(parent) { }
  virtual ~MediaWriter() { }

  virtual QString codecName() const = 0;

  virtual void finish() { }
  virtual void writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader) = 0;
};

class DummyMediaWriter : public MediaWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(DummyMediaWriter)
  typedef DummyMediaWriter Self;
  typedef MediaWriter Base;
public:
  explicit DummyMediaWriter(QObject *parent = 0) : Base(parent) { }

  virtual QString codecName() const { return "Dummy"; } ///< \override

  virtual void writeFrame(const QByteArray &, quint32, bool) { } ///< \override
};

class Mp3Writer : public DummyMediaWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(Mp3Writer)
  typedef DummyMediaWriter Base;
public:
  explicit Mp3Writer(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "MP3"; } ///< \override
};
class SpeexWriter : public DummyMediaWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(SpeexWriter)
  typedef DummyMediaWriter Base;
public:
  explicit SpeexWriter(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "Speex"; } ///< \override
};
class WavWriter : public DummyMediaWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(WavWriter)
  typedef DummyMediaWriter Base;
public:
  explicit WavWriter(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "WAV"; } ///< \override
};
class AviWriter : public DummyMediaWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(AviWriter)
  typedef DummyMediaWriter Base;
public:
  explicit AviWriter(QObject *parent = 0) : Base(parent) { }
  virtual QString codecName() const { return "AVI"; } ///< \override
};

class AacWriter : public MediaWriter {
  Q_OBJECT
  Q_DISABLE_COPY(AacWriter)
  typedef AacWriter Self;
  typedef MediaWriter Base;

  OutputStream *out_;
  MediaToc *toc_;
  int _aacProfile;
  int _sampleRateIndex;
  int _channelConfig;

public:
  explicit AacWriter(OutputStream *out, MediaToc *toc = 0, QObject *parent = 0)
    : Base(parent), out_(out), toc_(toc) { Q_ASSERT(out_); }

  virtual QString codecName() const { return "AAC"; } ///< \override

  virtual void writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader); ///< \override
  virtual void finish() { out_->finish(); } ///< \override
};

class RawH264Writer : public MediaWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(RawH264Writer)
  typedef RawH264Writer Self;
  typedef MediaWriter Base;

  OutputStream *out_;
  MediaToc *toc_;
  int nalLengthSize_; // 2 for 16bit or 4 for 32bit

public:
  explicit RawH264Writer(OutputStream *out, MediaToc *toc = 0, QObject *parent = 0)
    : Base(parent), out_(out), toc_(toc), nalLengthSize_(0) { Q_ASSERT(out_); }

  virtual QString codecName() const { return "H264"; } ///< \override

  virtual void writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader); ///< \override
  virtual void finish() { out_->finish(); } ///< \override
};

#endif // MEDIAWRITER_H
