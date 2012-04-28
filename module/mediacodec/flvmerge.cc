// flvmerge.cc
// 3/14/2012
// See: http://hackipedia.org/File%20formats/SWF,%20Macromedia-Adobe%20Flash/Adobe%20Flash%209%20File%20Format%20Specification.pdf
// Adobe Flash 9 File Format Specification.pdf (2007), The FLV Header (page 268)

#include "module/mediacodec/flvmerge.h"
#include "module/mediacodec/flvmeta.h"
#include "module/stream/datainputstream.h"

//#define DEBUG "flvmerge"
#include "module/debug/debug.h"

// - Merge -

void
FlvMerge::run()
{
  ins_.reset();
  if (!parse()) {
    emit error(tr("failed to parse FLV streams"));
    return;
  }
  ins_.reset();
  if (!merge()) {
    emit error(tr("failed to merge FLV streams"));
    return;
  }
  finish();
}

bool
FlvMerge::parse()
{
  DOUT("enter");
  Q_ASSERT(!ins_.isEmpty());
  if (ins_.isEmpty())
    return false;

  FlvMetaReader parser;
  //connect(&parser, SIGNAL(error(QString)), SIGNAL(error(QString)));
  //connect(this, SIGNAL(stopped()), &parser, SLOT(parse()));

  if (parser.parseStreams(ins_)) {
    FlvMeta meta = parser.meta();
    if (meta.duration)
      duration_ = meta.duration;
    DOUT("duration =" << duration_);
  }

  DOUT("exit");
  return true;
}

bool
FlvMerge::merge()
{
  DOUT("enter");
  audioTimestamp_ = videoTimestamp_ =
  lastAudioTimestamp_ = lastVideoTimestamp_ =
  lastAudioTimestep_ = lastVideoTimestep_ = 0;

  Q_ASSERT(!ins_.isEmpty());
  Q_ASSERT(out_);
  if (ins_.isEmpty() || !out_)
    return false;

  state_ = Running;

  bool writeHeader = true;
  foreach (InputStream *in, ins_) {
    Q_ASSERT(in);
    bool ok = append(in, writeHeader);
    if (!ok) {
      state_ = Error;
      break;
    }
    writeHeader = false;
  }
  if (isRunning())
    state_ = Finished;
  DOUT("exit: ret =" << isFinished());
  return isFinished();
}

// - FLV Header -
// Signature            UI8     'F'
// Signature            UI8     'L'
// Signature            UI8     'V'
// Version              UI8     FLV version, such as 0x01 for FLVv1
// TypeFlagsReserved    UB[5]   Must be 0
// TypeFlagsAudio       UB[1]   If audio presents, bool
// TypeFlagsReserved    UB[0]   Must be 0
// TypeFlagsVideo       UB[1]   If video presents, bool
// DataOffset           UI32    Offset from the beginning (9 if no padding)
//
// - FLV Body -
// PreviousTagSize0     UI32    Always 0
// Tag1                 FLVTAG  First tag
// PreviousTagSize1     UI32    sizeof(Tag1) (== Tag1.dataSize + 11)
// Tag2                 FLVTAG
// ...
// PreviousTagSizeN-1   FLVTAG
// TagN                 UI32    Last tag
// PreviousTagSizeN     FLVTAG  sizeof(TagN)
//
// - FLV Tag -
// TagType              UI8     8:audio, 9:video, 18:script
// DataSize             UI24
// TimeStamp            UI24    in msec
// TimeStampExtended    UI8
// StreamID             UI24    Always 0
// Data: AUDIODATA | VIDEODATA | SCRIPTDATA
//
// - FLV Data -
// MediaInfo            UI8
// ...
//
bool
FlvMerge::append(InputStream *in, bool writeHeader)
{
  DOUT("enter: writeHeader =" << writeHeader);
  Q_ASSERT(in);

  enum { HeaderSize = // 0x9
    + 4 // FLV tag
    + 1 // flags
    + 4 // data offset
  };
  QByteArray header(HeaderSize, 0);
  if (in->read(header) != HeaderSize)
    return false;
  DataInputStream headerIn(header);

  bool ok;
  if (headerIn.readUInt32() != 0x464C5601) {
    DOUT("exit: ERROR: this isn't a FLV file.");
    return false;
  }

  quint32 flags = headerIn.readUInt8();
  Q_UNUSED(flags); // supposed to be 0x5 for FLV with both a/v tracks
  DOUT("FLV flags =" << flags);

  qint64 dataOffset = headerIn.readUInt32();
  if (!dataOffset) {
    DOUT("exit: malformed FLV with zero dataOffset");
    return false;
  }

  if (writeHeader)
    out_->write(header);

  qint64 paddingSize = dataOffset - HeaderSize; // Often 0
  if (paddingSize > 0) {
    QByteArray padding(paddingSize, 0);
    if (in->read(padding) != paddingSize)
      return false;

    if (writeHeader)
      out_->write(padding);
  }

  quint32 prevTagSize = in->readUInt32(&ok);
  if (!ok)
    return false;
  if (prevTagSize)
    DOUT("warning: invalid FLV with non-zero first tag size:" << prevTagSize);

  if (writeHeader)
    out_->writeUInt32(prevTagSize); // first prevTagSize is always 0

  lastAudioTimestamp_ += lastAudioTimestep_;
  lastVideoTimestamp_ += lastAudioTimestep_;

  while (isRunning() && readTag(in, writeHeader)) ;

  Q_ASSERT(in->atEnd());

  //duration_ += timestamp_;
  lastAudioTimestamp_ = audioTimestamp_;
  lastVideoTimestamp_ = videoTimestamp_;

  DOUT("exit: ret =" << isRunning());
  return isRunning();
}

void
FlvMerge::finish()
{
  DOUT("enter");
  Q_ASSERT(out_);
  out_->finish();
  DOUT("exit");
}

bool
FlvMerge::readTag(InputStream *in, bool writeHeader)
{
  enum TagType { AudioTag = 8, VideoTag = 9, ScriptTag = 18 };

  enum { TagSize = // 11 for FLV v1
    + 1 // tagType
    + 3 // dataSize
    + 4 // timestamp
    + 3 // streamId
  };
  enum { TimestampOffset = 1 + 3 };

  QByteArray tagData(TagSize, 0); // header
  if (in->read(tagData) != TagSize)
    return false;
  DataInputStream tagIn(tagData);

  // Read tag header
  bool ok;
  quint32 tagType = tagIn.readUInt8();
  quint32 dataSize = tagIn.readUInt24();
  quint32 timestamp = tagIn.readUInt24();
  timestamp |= tagIn.readUInt8() << 24;
  //quint32 streamId = tagIn.readUInt24();

  quint32 newTimestamp = timestamp;
  switch (tagType) {
  case AudioTag:
    newTimestamp += lastAudioTimestamp_;
    lastAudioTimestep_ = newTimestamp - audioTimestamp_;
    audioTimestamp_ = newTimestamp;
    break;
  case VideoTag:
    newTimestamp += lastVideoTimestamp_;
    lastVideoTimestep_ = newTimestamp - videoTimestamp_;
    videoTimestamp_ = newTimestamp;
    break;
  }
  if (timestamp != newTimestamp) {
    quint8 ts[4];
    Bitwise::BigEndian::getBytes(ts, newTimestamp << 8);
    ts[3] = newTimestamp >> 24;
    ::memcpy(tagData.data() + TimestampOffset, ts, 4);
  }

  // Read tag data
  if (dataSize == 0) // no data
    return false;

  //quint32 mediaInfo = readUInt8(&tagIn, &ok); if (!ok) return false;
  //dataSize--;

  //QByteArray data = readBytes((int)dataSize);
  QByteArray data(dataSize, 0); // tag body
  if (in->read(data) != dataSize)
    return false;

  quint32 tagSize = in->readUInt32(&ok);
  if (!ok)
    return false;

  //emit timestampChanged(newTimestamp);

  // Data:
  // - MediaInfo        UI8     Flags indicating codec, frame/sample rate, etc
  // - First byte       UI8     Often the size of media data (0 for header)
  //bool isHeader = tagType == ScriptTag || !data[1];
  if (!writeHeader && (tagType == ScriptTag || !data[1])) {
    DOUT("drop header at time" << newTimestamp);
    return true;
  }

  //if (dropFooter && (timestamp == 364406 || timestamp == 364416 || timestamp == 364437)) {
  //  DOUT("drop footer at time" << newTimestamp);
  //  return true;
  //}

  if (tagType == ScriptTag)
    updateScriptTag(data, 1); // skip the first byte

  out_->write(tagData);
  out_->write(data);
  out_->writeUInt32(tagSize);

  return ok;
}

// onMetaData
// http://www.buraks.com/captionate/helpfile/e05.html
// duration (Number): Length of the FLV in seconds. Captionate computes this value. This is the timestamp of the last tag, plus the duration of the last audio tag taken into account.
// lasttimestamp (Number): This is the TimeStamp of the last tag in the FLV. (NetStream.Time will not return the duration, or the actual time as one might expect. It returns the position of the PlayHead which can only be a timestamp in the FLV file. So, lasttimestamp value is useful for detecting the end of the FLV more than the duration value).
// lastkeyframetimestamp (Number): The timestamp value of the last video key frame. This info might be needed because seeking a frame after this time usually does not work.
// width (Number): Width of the video in pixels.
// height (Number): Height of the video in pixels.
// videodatarate (Number): Captionate does not compute this value. If not present Captionate uses 0.
// audiodatarate (Number): Captionate does not compute this value. If not present Captionate uses 0.
// framerate (Number): Frames per second value. Captionate computes this value.
// creationdate (String): Captionate cannot compute this value. If not present Captionate uses 'unknown'.
// filesize (Number): File size in bytes (including all the injected data).
// videosize (Number): Total size of video tags in the file in bytes.
// audiosize (Number): Total size of audio tags in the file in bytes.
// datasize (Number): Total size of data tags in the file in bytes.
// metadatacreator (String): Set to 'Manitu Group Captionate 2.0'. (Note, this string will remain the same until Captionate version 3.0).
// metadatadate (Date): Date and time metadata added. (Note that this is not of type string like 'creationdate').
// audiocodecid (Number): Audio codec ID used in the FLV. (Captionate uses the first audio tag with non-zero data size for this value). Possible values are 0: Uncompressed, 1: ADPCM, 2: MP3, 5: Nellymoser 8kHz Mono, 6: Nellymoser.
// videocodecid (Number): Video codec ID used in the FLV (Captionate uses the first video tag for this value). Possible values are 2: Sorenson H.263, 3: Screen Video, 4: On2 VP6, 5: On2 VP6 with Transparency.
// audiodelay (Number): The delay introduced by the audio codec, in seconds. During encoding, other tags in the FLV are shifted by the same amount, in order to ensure accurate audio/video synchronization. Captionate calculates this value from the difference in time values of the the first audio tag and first video tag.
// canSeekToEnd (Boolean): Indicates whether the last video frame is a key frame or not.
// cuePoints (Array) : Lists all the cue points in the FLV. This is an array of objects which duplicate the information in the cue points and each object has the same properties: name, time, type and parameters. Captionate has an option for not saving 'event' type cue points into this array. See FLV Information Dialog, Metadata tab.
// keyframes (Object) : This object is added only if you select the option in FLV Information Dialog, Metadata tab. 'keyframes' object has 2 arrays: 'filepositions' and 'times'. Both arrays have the same number of elements, which is equal to the number of key frames in the FLV. Values in times array are in 'seconds'. Each correspond to the timestamp of the n'th key frame. Values in filepositions array are in 'bytes'. Each correspond to the fileposition of the nth key frame video tag (which starts with byte tag type 9).
// custommetadata (Object) : This object is added only if you select the option in FLV Information Dialog, Metadata tab. 'custommetadata' holds an associative array of name:value pair strings.
//
// http://stackoverflow.com/questions/1879195/get-flv-length-before-uploading-to-server
// duration: a DOUBLE indicating the total duration of the file in seconds
// width: a DOUBLE indicating the width of the video in pixels
// height: a DOUBLE indicating the height of the video in pixels
// videodatarate: a DOUBLE indicating the video bit rate in kilobits per second
// framerate: a DOUBLE indicating the number of frames per second
// videocodecid: a DOUBLE indicating the video codec ID used in the file (see “Video tags” on page 8 for available CodecID values)
// audiosamplerate: a DOUBLE indicating the frequency at which the audio stream is replayed
// audiosamplesize: a DOUBLE indicating the resolution of a single audio sample
// stereo: a BOOL indicating whether the data is stereo
// audiocodecid: a DOUBLE indicating the audio codec ID used in the file (see “Audio tags” on page 6 for available SoundFormat values)
// filesize: a DOUBLE indicating the total size of the file in bytes
int
FlvMerge::updateScriptTag(QByteArray &data, int pos) const
{
#define CHECK_OK        if (!ok) return -1
  enum ScriptDataType {
    DoubleType = 0,
    UInt8Type = 1,
    StringType = 2,
    StringType_Path = 4,
    UInt16Type = 7,
    ECMAArrayType = 8,
    DateType = 11,
    LongStringType = 12
  };
  if (pos < 0)
    return pos;
  bool ok;
  DataInputStream dataIn(data);
  InputStream &in = dataIn;
  ok = in.seek(pos);
  CHECK_OK;

  // - ScriptDataString
  // - ScriptDataValue
  quint16 stringLength = in.readUInt16(&ok); CHECK_OK;
  QByteArray stringData(stringLength, 0);
  ok = in.read(stringData) == stringLength; CHECK_OK;
  QString var(stringData);

  quint8 valueType = in.readUInt8(&ok); CHECK_OK;
  switch (valueType) {
  case DoubleType:
    {
      double value = in.readDouble(&ok); CHECK_OK;
      Q_UNUSED(value);
      DOUT("meta:" << var << "double" << value);
      int offset = in.pos() - sizeof(double);
      updateScriptTagDoubleValue((quint8 *)data.data() + offset, var);
    } break;
  case UInt8Type:
    {
      quint8 value = in.readUInt8(&ok); CHECK_OK;
      Q_UNUSED(value);
      DOUT("meta:" << var << "byte" << value);
      int offset = in.pos() - sizeof(quint8);
      updateScriptTagUInt8Value((quint8 *)data.data() + offset, var);
    } break;
  case UInt16Type:
    {
      quint16 value = in.readUInt16(&ok); CHECK_OK;
      Q_UNUSED(value);
      DOUT("meta:" << var << "short" << value);
    } break;
  case DateType:
    {
      in.readUInt64(&ok); CHECK_OK;
      in.readUInt16(&ok); CHECK_OK;
      DOUT("meta:" << var << "date");
    } break;
  case StringType:
  case StringType_Path:
    {
      quint16 valueLength = in.readUInt16(&ok); CHECK_OK;
      QByteArray valueData(valueLength, 0);
      ok = in.read(valueData) == valueLength; CHECK_OK;
      QString value(valueData);
      Q_UNUSED(value);
      DOUT("meta:" << var << "string" << value);
    } break;
  case LongStringType:
    {
      quint32 valueLength = in.readUInt32(&ok); CHECK_OK;
      QByteArray valueData(valueLength, 0);
      ok = in.read(valueData) == valueLength; CHECK_OK;
      QString value(valueData);
      Q_UNUSED(value);
      DOUT("meta:" << var << "lstring" << value);
    } break;
  case ECMAArrayType:
    {
      quint32 valueCount = in.readUInt32(&ok); CHECK_OK;
      DOUT("meta:" << var << "array" << valueCount);
      int offset = in.pos();
      for (quint32 i = 0; i < valueCount; i++) {
        offset = updateScriptTag(data, offset);
        if (pos < 0)
          return pos;
      }
      ok = in.seek(pos); CHECK_OK;
    }
  }
  return in.pos();
#undef CHECK_OK
}

bool
FlvMerge::updateScriptTagDoubleValue(quint8 *data, const QString &var) const
{
  enum { size = sizeof(double) };
  bool update = false;
  double value;
  bool zero = false;
  // TODO: use qHash to improve performance
  if (var == "duration") {
    update = true;
    value = duration_ / 1000.0;
  } else if (var == "duration" ||
      var == "datasize" ||
      var == "videosize" ||
      var == "audiosize" ||
      var == "lasttimestamp" ||
      var == "lastkeyframetimestamp" ||
      var == "lastkeyframelocation") {
    update = true;
    zero = true;
  }

  if (update) {
    DOUT(var << "=" << value);
    quint8 bytes[size] = { };
    if (!zero)
      Bitwise::BigEndian::getBytes(bytes, value);
    ::memcpy(data, bytes, size);
  }
  return update;
}

bool
FlvMerge::updateScriptTagUInt8Value(quint8 *data, const QString &var) const
{
  Q_UNUSED(data);
  Q_UNUSED(var);
  return false;
/*
  bool update = false;
  quint8 value;
  if ((var == "hasKeyframes" ||
       var == "hasMetadata" ||
       var == "canSeekToEnd"
      ) && *data) {
    update = true;
    value = 0;
  }

  if (update) {
    DOUT(var << "=" << value);
    *data = value;
  }
  return update;
*/
}

// EOF
