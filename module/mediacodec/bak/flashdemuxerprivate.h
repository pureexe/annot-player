#ifndef FLASHDEMUXERPRIVATE_H
#define FLASHDEMUXERPRIVATE_H

// flashdemuxerprivate.h
// 2/12/2012
//
// See: FLVExtractCl/Library/FLVFile.cs
// See: FLVExtractCl/Library/General.cs
// http://moitah.net/

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QDataStream>

//#define DEBUG "flashdemuxerprivate"
#include "module/debug/debug.h"

namespace BitHelper
{
  inline int read(quint64 &x, int length)
  {
    int r = (int)(x >> (64 - length));
    x <<= length;
    return r;
  }

  inline int read(const quint32 *bytes, int &offset, int length)
  {
    int startByte = offset / 8;
    int endByte = (offset + length - 1) / 8;
    int skipBits = offset % 8;
    quint64 bits = 0;
    for (int i = 0; i <= qMin(endByte - startByte, 7); i++) {
      bits |= quint64(bytes[startByte + i]) << (56 - (i * 8));
    }
    if (skipBits)
      read(bits, skipBits);
    offset += length;
    return read(bits, length);
  }

  inline void write(quint64 &x, int length, int value)
  {
    quint64 mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
    x = (x << length) | (quint64(value) & mask);
  }
}

namespace BitConverterBE
{
  inline quint64 toUInt64(const quint8 *value, int startIndex = 0)
  {
    return
      (quint64(value[startIndex    ]) << 56) |
      (quint64(value[startIndex + 1]) << 48) |
      (quint64(value[startIndex + 2]) << 40) |
      (quint64(value[startIndex + 3]) << 32) |
      (quint64(value[startIndex + 4]) << 24) |
      (quint64(value[startIndex + 5]) << 16) |
      (quint64(value[startIndex + 6]) <<  8) |
      (quint64(value[startIndex + 7])      );
  }

  inline quint32 toUInt32(const quint8 *value, int startIndex = 0)
  {
    return
      ((quint32)value[startIndex    ] << 24) |
      ((quint32)value[startIndex + 1] << 16) |
      ((quint32)value[startIndex + 2] <<  8) |
      ((quint32)value[startIndex + 3]      );
  }

  inline quint16 toUInt16(const quint8 *value, int startIndex = 0)
  {
    return
      ((quint16)value[startIndex    ] <<  8) |
      ((quint16)value[startIndex + 1]      );
  }

  inline void getBytes(quint8 *buff, quint64 value)
  {
    buff[0] = (quint8)(value >> 56);
    buff[1] = (quint8)(value >> 48);
    buff[2] = (quint8)(value >> 40);
    buff[3] = (quint8)(value >> 32);
    buff[4] = (quint8)(value >> 24);
    buff[5] = (quint8)(value >> 16);
    buff[6] = (quint8)(value >>  8);
    buff[7] = (quint8)(value      );
  }

  inline void getBytes(quint8 *buff, quint32 value)
  {
    buff[0] = (quint8)(value >> 24);
    buff[1] = (quint8)(value >> 16);
    buff[2] = (quint8)(value >>  8);
    buff[3] = (quint8)(value      );
  }

  inline void getBytes(quint8 *buff, quint16 value)
  {
    buff[0] = (quint8)(value >>  8);
    buff[1] = (quint8)(value      );
  }
}

namespace BitConverterLE {
  inline void getBytes(quint8 *buff, quint64 value)
  {
    buff[0] = (quint8)(value      );
    buff[1] = (quint8)(value >>  8);
    buff[2] = (quint8)(value >> 16);
    buff[3] = (quint8)(value >> 24);
    buff[4] = (quint8)(value >> 32);
    buff[5] = (quint8)(value >> 40);
    buff[6] = (quint8)(value >> 48);
    buff[7] = (quint8)(value >> 56);
  }

  inline void getBytes(quint8 *buff, quint32 value)
  {
    buff[0] = (quint8)(value      );
    buff[1] = (quint8)(value >>  8);
    buff[2] = (quint8)(value >> 16);
    buff[3] = (quint8)(value >> 24);
  }

  inline void getBytes(quint8 *buff, quint16 value)
  {
    buff[0] = (quint8)(value      );
    buff[1] = (quint8)(value >>  8);
  }
}

struct FractionUInt32
{
  quint32 N;
  quint32 D;

public:
  FractionUInt32() : N(0), D(0) { }
  FractionUInt32(quint32 n, quint32 d) : N(n), D(d) { }

  double toDouble() const { return double(N) / D; }
  bool isNull() const { return !N && !D; }

  void reduce()
  {
    quint32 gcd = GCD(N, D);
    N /= gcd;
    D /= gcd;
  }

private:
  static quint32 GCD(quint32 a, quint32 b)
  {
    quint32 r;
    while (b != 0) {
      r = a % b;
      a = b;
      b = r;
    }
    return a;
   }
};

class IAudioWriter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(IAudioWriter)
  typedef IAudioWriter Self;
  typedef QObject Base;

  QString path_;
public:
  explicit IAudioWriter(QObject *parent = nullptr) : Base(parent) { }
  virtual ~IAudioWriter() { }

  const QString &path() const { return path_; }
  void setPath(const QString &path) { path_ = path; }

  virtual void finish() { }
  virtual void writeChunk(const QByteArray &chunk, quint32 timeStamp) = 0;
};

class IVideoWriter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(IVideoWriter)
  typedef IVideoWriter Self;
  typedef QObject Base;

  QString path_;
public:
  explicit IVideoWriter(QObject *parent = nullptr) : Base(parent) { }
  virtual ~IVideoWriter() { }

  const QString &path() const { return path_; }
  void setPath(const QString &path) { path_ = path; }

  virtual void finish(const FractionUInt32 &averageFrameRate) { Q_UNUSED(averageFrameRate) }
  virtual void writeChunk(const QByteArray &chunk, quint32 timeStamp, int frameType) = 0;
};

class DummyAudioWriter : public IAudioWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(DummyAudioWriter)
  typedef DummyAudioWriter Self;
  typedef IAudioWriter Base;
public:
  explicit DummyAudioWriter(QObject *parent = nullptr) : Base(parent) { }

  virtual void writeChunk(const QByteArray &, quint32) { }
  virtual void finish() { }
};


class DummyVideoWriter : public IVideoWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(DummyVideoWriter)
  typedef DummyVideoWriter Self;
  typedef IVideoWriter Base;
public:
  explicit DummyVideoWriter(QObject *parent = nullptr) : Base(parent) { }

  virtual void writeChunk(const QByteArray &, quint32, int) { }
  virtual void finish(const FractionUInt32 &) { }
};

typedef DummyAudioWriter MP3Writer;
typedef DummyAudioWriter SpeexWriter;
typedef DummyAudioWriter WAVWriter;
typedef DummyVideoWriter AVIWriter;

class AACWriter : public IAudioWriter {
  Q_OBJECT
  Q_DISABLE_COPY(AACWriter)
  typedef AACWriter Self;
  typedef IAudioWriter Base;

  QString _path;
  QFile *file_;
  int _aacProfile;
  int _sampleRateIndex;
  int _channelConfig;

public:
  explicit AACWriter(const QString &path, QObject *parent = nullptr)
    : Base(parent), _path(path)
  {
    DOUT("enter: path =" << _path);
    file_ = new QFile(_path, this);
    bool ok = file_->open(QIODevice::WriteOnly);
    Q_UNUSED(ok)
    DOUT("exit: isOpen =" << ok);
  }

  virtual void writeChunk(const QByteArray &chunk, quint32 timeStamp)
  {
    Q_UNUSED(timeStamp)
    if (chunk.size() < 1) return;

    if (chunk[0] == 0) { // Header
       if (chunk.size() < 3) return;

       const quint8 *p = (const quint8 *)chunk.data();
       quint64 bits = quint64(BitConverterBE::toUInt16(p, 1)) << 48;

       _aacProfile = BitHelper::read(bits, 5) - 1;
       _sampleRateIndex = BitHelper::read(bits, 4);
       _channelConfig = BitHelper::read(bits, 4);

       if ((_aacProfile < 0) || (_aacProfile > 3)) {
          DOUT("Unsupported AAC profile.");
          return;
       }
       if (_sampleRateIndex > 12) {
         DOUT("Invalid AAC sample rate index.");
         return;
       }
       if (_channelConfig > 6) {
         DOUT("Invalid AAC channel configuration.");
         return;
       }
     }
     else { // Audio data
       int dataSize = chunk.size() - 1;
       quint64 bits = 0;

       // Reference: WriteADTSHeader from FAAC's bitstream.c

       BitHelper::write(bits, 12, 0xFFF);
       BitHelper::write(bits,  1, 0);
       BitHelper::write(bits,  2, 0);
       BitHelper::write(bits,  1, 1);
       BitHelper::write(bits,  2, _aacProfile);
       BitHelper::write(bits,  4, _sampleRateIndex);
       BitHelper::write(bits,  1, 0);
       BitHelper::write(bits,  3, _channelConfig);
       BitHelper::write(bits,  1, 0);
       BitHelper::write(bits,  1, 0);
       BitHelper::write(bits,  1, 0);
       BitHelper::write(bits,  1, 0);
       BitHelper::write(bits, 13, 7 + dataSize);
       BitHelper::write(bits, 11, 0x7FF);
       BitHelper::write(bits,  2, 0);

       quint8 bytes[8];
       BitConverterBE::getBytes(bytes, bits);
       write(bytes, 1, 7);
       write(chunk, 1, dataSize);
    }
  }

  virtual void finish()
  { if (file_->isOpen()) file_->close(); }

  void write(const char *data, int offset, int count)
  { file_->write(data + offset, count); }

  void write(const quint8 *data, int offset, int count)
  { write((const char *)data, offset, count); }
};

class RawH264Writer : public IVideoWriter
{
  Q_OBJECT
  Q_DISABLE_COPY(RawH264Writer)
  typedef RawH264Writer Self;
  typedef IVideoWriter Base;

  QString _path;
  QFile *file_;
  int _nalLengthSize;

public:
  explicit RawH264Writer(const QString &path, QObject *parent)
    : Base(parent), _path(path)
  {
    DOUT("enter: path =" << _path);
    file_ = new QFile(_path, this);
    bool ok = file_->open(QIODevice::WriteOnly);
    Q_UNUSED(ok)
    DOUT("exit: isOpen =" << ok);
  }

  virtual void writeChunk(const QByteArray &chunk, quint32 timeStamp, int frameType)
  {
    Q_UNUSED(timeStamp)
    Q_UNUSED(frameType)
    static const quint8 _startCode[4] = { 0, 0, 0, 1 };
    enum { _startCodeSize = sizeof(_startCode)/sizeof(*_startCode) };
    if (chunk.size() < 4)
      return;

    // Reference: decode_frame from libavcodec's h264.c

    if (chunk[0] == 0) { // Headers
      if (chunk.size() < 10)
        return;

      int offset = 8;
      _nalLengthSize = (chunk[offset++] & 0x03) + 1;
      int spsCount = chunk[offset++] & 0x1F;
      int ppsCount = -1;

      while (offset <= chunk.size() - 2) {
        if ((spsCount == 0) && (ppsCount == -1)) {
          ppsCount = chunk[offset++];
          continue;
        }

        if (spsCount > 0) spsCount--;
        else if (ppsCount > 0) ppsCount--;
        else break;

        const quint8 *p = (const quint8 *)chunk.data();
        int len = (int)BitConverterBE::toUInt16(p, offset);
          offset += 2;
        if (offset + len > chunk.size()) break;
        write(_startCode, 0, _startCodeSize);
        write(chunk, offset, len);
        offset += len;
      }
    } else { // Video data
      int offset = 4;

      if (_nalLengthSize != 2)
        _nalLengthSize = 4;

      const quint8 *p = (const quint8 *)chunk.data();
      while (offset <= chunk.size() - _nalLengthSize) {
        int len = (_nalLengthSize == 2) ?
          (int)BitConverterBE::toUInt16(p, offset) :
          (int)BitConverterBE::toUInt32(p, offset);
        offset += _nalLengthSize;
        if (offset + len > chunk.size()) break;
        write(_startCode, 0, _startCodeSize);
        write(chunk, offset, len);
        offset += len;
      }
    }
  }

  virtual void finish(const FractionUInt32 &averageFrameRate)
  {
    Q_UNUSED(averageFrameRate)
    if (file_->isOpen()) file_->close();
  }

  void write(const char *data, int offset, int count)
  { file_->write(data + offset, count); }

  void write(const quint8 *data, int offset, int count)
  { write((const char *)data, offset, count); }
};

class FlvFile : QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(FlvFile)
  typedef FlvFile Self;
  typedef QObject Base;

  //static readonly string[] _outputExtensions = new string[] { ".avi", ".mp3", ".264", ".aac", ".spx", ".txt" };
  QString _inputPath, _outputDirectory, _outputPathBase;

  QFile *file_;

  qint64 _fileOffset, _fileLength;
  IAudioWriter *_audioWriter;
  IVideoWriter *_videoWriter;
  //TimeCodeWriter _timeCodeWriter;
  QList<quint32> _videoTimeStamps;
  bool _extractAudio, _extractVideo, _extractTimeCodes;
  bool _extractedAudio, _extractedVideo, _extractedTimeCodes;
  FractionUInt32 _averageFrameRate, _trueFrameRate;
  QList<QString> _warnings;

public:
  explicit FlvFile(QString path, QObject *parent = nullptr)
    : Base(parent),
      _inputPath(path), _fileOffset(0), _fileLength(0),
      _audioWriter(nullptr), _videoWriter(nullptr)
  {
    DOUT("enter: path =" << path);
    _outputDirectory = QFileInfo(path).absolutePath();
    //_fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read, 65536);
    {
      file_ = new QFile(_inputPath, this);
      if (!file_->open(QIODevice::ReadOnly)) {
        DOUT("failed to open file");
        return;
      }
      _fileLength = file_->size();
    }
    DOUT("exit: fileSize =" << _fileLength);
  }

public:
  void extractStreams(bool extractAudio = true, bool extractVideo = true, bool extractTimeCodes = true)
  {
    DOUT("enter");
    _outputPathBase = "/Users/jichi/tmp/test";
    _extractAudio = extractAudio;
    _extractVideo = extractVideo;
    _extractTimeCodes = extractTimeCodes;
    _videoTimeStamps.clear();

    if (_fileLength <= 0) {
      DOUT("failed to open file or empty file");
      return;
    }

    seek(0);

    if (_fileLength < 4 || readUInt32() != 0x464C5601) {
      if (_fileLength >= 8 && readUInt32() == 0x66747970) {
        DOUT("This is a MP4 file. YAMB or MP4Box can be used to extract streams.");
        return;
      }
      else {
        DOUT("This isn't a FLV file.");
        return;
      }
    }

    //if (Array.IndexOf(_outputExtensions, Path.GetExtension(_inputPath).ToLowerInvariant()) != -1) {
    //    // Can't have the same extension as files we output
    //    throw new Exception("Please change the extension of this FLV file.");
    //}

    //if (!Directory.Exists(_outputDirectory)) {
    //    throw new Exception("Output directory doesn't exist.");
    //}

    quint32 flags = readUInt8();
    Q_UNUSED(flags)
    quint32 dataOffset = readUInt32();

    seek(dataOffset);

    quint32 prevTagSize = readUInt32();
    while (_fileOffset < _fileLength) {
      if (!readTag()) break;
      if ((_fileLength - _fileOffset) < 4) break;
      prevTagSize = readUInt32();
    }

    _averageFrameRate = calculateAverageFrameRate();
    _trueFrameRate = calculateTrueFrameRate();

    closeOutput(_averageFrameRate, false);
    DOUT("exit");
  }

private:
  void closeOutput(FractionUInt32 averageFrameRate, bool disposing)
  {
    DOUT("enter: videoWriter =" << _videoWriter << ", audioWriter =" << _audioWriter);
    Q_UNUSED(disposing)
    if (_videoWriter)
      _videoWriter->finish(averageFrameRate.isNull() ? FractionUInt32(25, 1) : averageFrameRate);
    if (_audioWriter)
      _audioWriter->finish();
    DOUT("exit");
  }

  bool readTag()
  {
    if ((_fileLength - _fileOffset) < 11)
      return false;

    // Read tag header
    quint32 tagType = readUInt8();
    quint32 dataSize = readUInt24();
    quint32 timeStamp = readUInt24(); timeStamp |= readUInt8() << 24;
    quint32 streamID = readUInt24();
    Q_UNUSED(streamID)

    // Read tag data
    if (dataSize == 0)
      return true;
    if ((_fileLength - _fileOffset) < dataSize)
      return false;
    quint32 mediaInfo = readUInt8();
    dataSize -= 1;

    //QByteArray data = readBytes((int)dataSize);
    QByteArray data((int)dataSize, '0');
    read(data.data(), 0, dataSize);

    if (tagType == 0x8) {  // Audio
      if (!_audioWriter) {
        _audioWriter = _extractAudio ? getAudioWriter(mediaInfo) : new DummyAudioWriter(this);
        _extractedAudio = !(qobject_cast<DummyAudioWriter *>(_audioWriter));
      }
      _audioWriter->writeChunk(data, timeStamp);

    } else if ((tagType == 0x9) && ((mediaInfo >> 4) != 5)) { // Video
      if (!_videoWriter) {
        _videoWriter = _extractVideo ? getVideoWriter(mediaInfo) : new DummyVideoWriter(this);
        _extractedVideo = !(qobject_cast<DummyVideoWriter *>(_videoWriter));
      }
      //if (_timeCodeWriter == null) {
      //  string path = _outputPathBase + ".txt";
      //  _timeCodeWriter = new TimeCodeWriter((_extractTimeCodes && CanWriteTo(path)) ? path : null);
      //  _extractedTimeCodes = _extractTimeCodes;
      //}
      _videoTimeStamps.append(timeStamp);
      _videoWriter->writeChunk(data, timeStamp, (int)((mediaInfo & 0xF0) >> 4));
      //_timeCodeWriter.Write(timeStamp);
    }
    return true;
  }

  IAudioWriter *getAudioWriter(quint32 mediaInfo)
  {
    DOUT("enter: mediaInfo =" << mediaInfo);
    quint32 format = mediaInfo >> 4;
    quint32 rate = (mediaInfo >> 2) & 0x3;
    quint32 bits = (mediaInfo >> 1) & 0x1; Q_UNUSED(bits)
    quint32 chans = mediaInfo & 0x1; Q_UNUSED(chans)
    QString path;

    if ((format == 2) || (format == 14)) { // MP3
      DOUT("MP3");
      path = _outputPathBase + ".mp3";
      //return new MP3Writer(path, _warnings);
      return new MP3Writer(this);
    } else if ((format == 0) || (format == 3)) { // PCM
      DOUT("PCM");
      int sampleRate = 0;
      switch (rate) {
      case 0: sampleRate =  5512; break;
      case 1: sampleRate = 11025; break;
      case 2: sampleRate = 22050; break;
      case 3: sampleRate = 44100; break;
      }
      path = _outputPathBase + ".wav";
      if (format == 0)
        _warnings.append("PCM byte order unspecified, assuming little endian.");
      //return new WAVWriter(path, (bits == 1) ? 16 : 8, (chans == 1) ? 2 : 1, sampleRate);
      return new WAVWriter(this);
    } else if (format == 10) { // AAC
      DOUT("AAC");
      path = _outputPathBase + ".aac";
      //return new AACWriter(path);
      return new AACWriter(path, this);
    } else if (format == 11) { // Speex
      DOUT("SPX");
      path = _outputPathBase + ".spx";
      //return new SpeexWriter(path, (int)(_fileLength & 0xFFFFFFFF));
      return new SpeexWriter(this);
    } else {
      QString typeStr;

      if (format == 1)
        typeStr = "ADPCM";
      else if ((format == 4) || (format == 5) || (format == 6))
        typeStr = "Nellymoser";
      else
        typeStr = "format=" + QString::number(format);
      DOUT(typeStr);

      _warnings.append("Unable to extract audio (" + typeStr + " is unsupported).");

      return new DummyAudioWriter(this);
    }
  }

  IVideoWriter *getVideoWriter(quint32 mediaInfo)
  {
    DOUT("enter: mediaInfo =" << mediaInfo);
    quint32 codecID = mediaInfo & 0x0F;
    QString path;

    if ((codecID == 2) || (codecID == 4) || (codecID == 5)) {
      path = _outputPathBase + ".avi";
      //return new AVIWriter(path, (int)codecID, _warnings);
      DOUT("AVI");
      return new AVIWriter(this);
    } else if (codecID == 7) {
      path = _outputPathBase + ".264";
      DOUT("H.264");
      return new RawH264Writer(path, this);
    } else {
      QString typeStr;

      if (codecID == 3)
       typeStr = "Screen";
      else if (codecID == 6)
        typeStr = "Screen2";
      else
        typeStr = "codecID=" + QString::number(codecID);
      DOUT(typeStr);

      _warnings.append("Unable to extract video (" + typeStr + " is unsupported).");

      return new DummyVideoWriter(this);
    }
  }

  FractionUInt32 calculateAverageFrameRate()
  {
     FractionUInt32 frameRate;
     int frameCount = _videoTimeStamps.size();

     if (frameCount > 1) {
         frameRate.N = (quint32)(frameCount - 1) * 1000;
         frameRate.D = _videoTimeStamps[frameCount - 1] - _videoTimeStamps[0];
         frameRate.reduce();
         return frameRate;
     }
     else {
         return FractionUInt32();
     }
 }

  FractionUInt32 calculateTrueFrameRate() {
    FractionUInt32 frameRate;
    QHash<quint32, quint32> deltaCount;
    int i, threshold;
    quint32 delta, count, minDelta;

    // Calculate the distance between the timestamps, count how many times each delta appears
    for (i = 1; i < _videoTimeStamps.size(); i++) {
      int deltaS = (int)((long)_videoTimeStamps[i] - (long)_videoTimeStamps[i - 1]);

      if (deltaS <= 0) continue;
      delta = (quint32)deltaS;

      if (deltaCount.contains(delta))
        deltaCount[delta] += 1;
      else
        deltaCount[delta] = 1;
    }

    threshold = _videoTimeStamps.size() / 10;
    minDelta = UINT32_MAX; //UInt32.MaxValue;

    // Find the smallest delta that made up at least 10% of the frames (grouping in delta+1
    // because of rounding, e.g. a NTSC video will have deltas of 33 and 34 ms)
    foreach (quint32 k, deltaCount.keys()) {
      delta = k;
      count = deltaCount[k];

      if (deltaCount.contains(delta + 1))
        count += deltaCount[delta + 1];

      if (((int)count >= threshold) && (delta < minDelta))
        minDelta = delta;
    }

    // Calculate the frame rate based on the smallest delta, and delta+1 if present
    if (minDelta != UINT32_MAX) {
      quint32 totalTime, totalFrames;

      count = deltaCount[minDelta];
      totalTime = minDelta * count;
      totalFrames = count;

      if (deltaCount.contains(minDelta + 1)) {
        count = deltaCount[minDelta + 1];
        totalTime += (minDelta + 1) * count;
        totalFrames += count;
      }

      if (totalTime != 0) {
        frameRate.N = totalFrames * 1000;
        frameRate.D = totalTime;
        frameRate.reduce();
        return frameRate;
      }
    }

    // Unable to calculate frame rate
    //return null;
    return FractionUInt32();
  }

  void seek(qint64 offset) {
    file_->seek(offset);
    _fileOffset = offset;
  }

 quint32 readUInt8() {
   quint8 byte;
   read(&byte, 0, 1);
   return (quint32)byte;
 }

 quint32 readUInt24() {
   quint8 x[4];
   x[0] = 0;
   read(x, 1, 3);
   return BitConverterBE::toUInt32(x, 0);
  }

  quint32 readUInt32() {
    quint8 x[4];
    read(x, 0, 4);
    return BitConverterBE::toUInt32(x, 0);
  }

  //char *readBytes(int length) {
  //  char *buff = new char[length];
  //  read(buff, 0, length);
  //  return buff;
  //}

  void read(quint8 *data, int offset, int count)
  { read((char *)data, offset, count); }
  void read(char *data, int offset, int count)
  {
    file_->read(data + offset, count);
    _fileOffset += count;
  }
};

#endif // FLASHDEMUXERPRIVATE_H
