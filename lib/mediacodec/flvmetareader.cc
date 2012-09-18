// flvmetareader.cc
// 3/16/2012
// See: http://hackipedia.org/File%20formats/SWF,%20Macromedia-Adobe%20Flash/Adobe%20Flash%209%20File%20Format%20Specification.pdf
// Adobe Flash 9 File Format Specification.pdf (2007), The FLV Header (page 268)

#include "lib/mediacodec/flvmeta.h"
#include "lib/stream/datainputstream.h"

//#define DEBUG "flvmetareader"
#include "qtx/qxdebug.h"

#define CHECK_OK        if (!ok) return false

// - Reader -

void
FlvMetaReader::run()
{
  DOUT("enter");
  if (!parse())
    emit errorMessage(tr("failed to parse FLV"));
  DOUT("exit");
}

bool
FlvMetaReader::parse()
{
  DOUT("enter");
  meta_.clear();

  Q_ASSERT(!ins_.isEmpty());
  if (ins_.isEmpty())
    return true;

  state_ = Running;

  foreach (InputStream *in, ins_) {
    Q_ASSERT(in);
    bool ok = readStream(in);
    if (!ok) {
      state_ = Error;
      break;
    }
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
FlvMetaReader::readStream(InputStream *in)
{
  DOUT("enter");
  Q_ASSERT(in);

  enum { HeaderSize = // 0x9
    + 4 // FLV tag
    + 1 // flags
    + 4 // data offset
  };
  //QByteArray header(HeaderSize, 0);
  //if (in->read(header) != HeaderSize)
  //  return false;

  bool ok;
  if (in->readUInt32(&ok) != 0x464C5601 || !ok) {
    DOUT("exit: ERROR: this isn't a FLV file.");
    return false;
  }

  quint32 flags = in->readUInt8(&ok); CHECK_OK;
  Q_UNUSED(flags) // supposed to be 0x5 for FLV with both a/v tracks
  DOUT("FLV flags =" << flags);

  qint64 dataOffset = in->readUInt32(&ok); CHECK_OK;
  if (!dataOffset) {
    DOUT("exit: malformed FLV with zero dataOffset");
    return false;
  }

  qint64 paddingSize = dataOffset - HeaderSize; // Often 0
  if (paddingSize > 0 &&
      in->skip(paddingSize) != paddingSize)
    return false;

  quint32 firstTagSize = in->readUInt32(&ok); CHECK_OK;
  if (firstTagSize)
    DOUT("warning: invalid FLV with non-zero first tag size:" << firstTagSize);

  ok = seekToScriptTag(in) && readScriptTag(in);
  if (!ok)
    state_ = Error;

  DOUT("exit: ret =" << isRunning());
  return isRunning();
}

bool
FlvMetaReader::seekToScriptTag(InputStream *in)
{
  enum TagType { AudioTag = 8, VideoTag = 9, ScriptTag = 18 };

  enum { TagSize = // 11 for FLV v1
    + 1 // tagType
    + 3 // dataSize
    + 4 // timestamp
    + 3 // streamId
  };

  QByteArray tagData(TagSize, 0); // header
  if (in->read(tagData) != TagSize)
    return false;
  DataInputStream tagIn(tagData);

  // Read tag header
  quint32 tagType = tagIn.readUInt8();
  quint32 dataSize = tagIn.readUInt24();
  //quint32 timestamp = tagIn.readUInt24();
  //timestamp |= tagIn.readUInt8() << 24;
  //quint32 streamId = tagIn.readUInt24();

  // Read tag data
  if (dataSize == 0) // no data
    return false;

  //quint32 mediaInfo = tagIn.readUInt8();
  //dataSize--;

  if (tagType != ScriptTag) // not script tag
    return true;

  if (in->skip(1) != 1) // skip first byte
    return false;

  //QByteArray data = readBytes((int)dataSize);
  //if (in->skip(dataSize) != dataSize)
  //  return false;

  //quint32 tagSize = readUInt32(in, &ok);
  //Q_UNUSED(tagSize)
  //if (!ok)
  //  return false;
  //parseScriptTag(data, 1); // skip the first byte

  return true;
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
bool
FlvMetaReader::readScriptTag(InputStream *in)
{
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
  bool ok = true;

  // - ScriptDataString
  // - ScriptDataValue
  quint16 stringLength = in->readUInt16(&ok); CHECK_OK;
  QByteArray stringData(stringLength, 0);
  ok = in->read(stringData) == stringLength; CHECK_OK;
  QString var(stringData);

  quint8 valueType = in->readUInt8(&ok); CHECK_OK;
  switch (valueType) {
  case DoubleType:
    {
      double value = in->readDouble(&ok); CHECK_OK;
      setMetaDouble(var, value);
    } break;
  case UInt8Type:
    {
      quint8 value = in->readUInt8(&ok); CHECK_OK;
      setMetaUInt8(var, value);
    } break;
  case UInt16Type:
    {
      quint16 value = in->readUInt16(&ok); CHECK_OK;
      Q_UNUSED(value)
      DOUT("meta:" << var << "short" << value);
    } break;
  case DateType:
    {
      in->readUInt64(&ok); CHECK_OK;
      in->readUInt16(&ok); CHECK_OK;
      DOUT("meta:" << var << "date");
    } break;
  case StringType:
  case StringType_Path:
    {
      quint16 valueLength = in->readUInt16(&ok); CHECK_OK;
      QByteArray valueData(valueLength, 0);
      ok = in->read(valueData) == valueLength; CHECK_OK;
      QString value(valueData);
      setMetaString(var, value);
    } break;
  case LongStringType:
    {
      quint32 valueLength = in->readUInt32(&ok); CHECK_OK;
      QByteArray valueData(valueLength, 0);
      ok = in->read(valueData) == valueLength; CHECK_OK;
      QString value(valueData);
      Q_UNUSED(value)
      DOUT("meta:" << var << "lstring" << value);
    } break;
  case ECMAArrayType:
    {
      quint32 valueCount = in->readUInt32(&ok); CHECK_OK;
      DOUT("meta:" << var << "array" << valueCount);
      for (quint32 i = 0; i < valueCount; i++) {
        ok = readScriptTag(in);
        CHECK_OK;
      }
    }
  }
  return ok;
}

void
FlvMetaReader::setMetaDouble(const QString &var, double value)
{
  DOUT("var =" << var << ", value =" << value);
#define ELIF_VAR(_var, _meta) \
  else if (var == _var) { \
    if (!meta_._meta) \
      meta_._meta = value; \
  }

  if (var == "duration") {
    qint64 duration = value * 1000;
    meta_.duration += duration;
  } else if (var == "lasttimestamp") {
    if (!meta_.lastTimestamp)
      meta_.lastTimestamp = value * 1000;
  } else if (var == "lastkeyframetimestamp") {
    if (!meta_.lastKeyFrameTimestamp)
      meta_.lastKeyFrameTimestamp = value * 1000;
  } else if (var == "audiodatarate")
    meta_.audioDataRate = value;
  else if (var == "videodatarate")
    meta_.videoDataRate = value;
  else if (var == "framerate")
    meta_.frameRate = value;
  ELIF_VAR("width", width)
  ELIF_VAR("height", height)
  ELIF_VAR("audiocodecid", audioCodecId)
  ELIF_VAR("videocodecid", videoCodecId)
  ELIF_VAR("filesize", fileSize)
  ELIF_VAR("audiosize", audioSize)
  ELIF_VAR("videosize", videoSize)
  ELIF_VAR("datasize", dataSize)
  ELIF_VAR("lastkeyframelocation", lastKeyFrameLocation)

#undef ELIF_VAR
}

void
FlvMetaReader::setMetaString(const QString &var, const QString &value)
{
  DOUT("var =" << var << ", value =" << value);
  if (var == "creator") {
    if (meta_.creator.isEmpty())
      meta_.creator = value;
  } else if (var == "metadatacreator") {
    if (meta_.metaDataCreator.isEmpty())
      meta_.metaDataCreator = value;
  }
}

void
FlvMetaReader::setMetaUInt8(const QString &var, quint8 value)
{
  Q_UNUSED(var)
  Q_UNUSED(value)
  DOUT("var =" << var << ", value =" << value);
}

// EOF

/*
bool
FlvMetaReader::updateScriptTagDoubleValue(quint8 *data, const QString &var) const
{
  enum { size = sizeof(double) };
  bool update = false;
  double value;
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
    value = 0;
  }

  if (update) {
    DOUT(var << "=" << value);
    quint8 bytes[size];
    QxBitwise::BigEndian::getBytes(bytes, value);
    qMemCopy(data, bytes, size);
  }
  return update;
}

bool
FlvMetaReader::updateScriptTagUInt8Value(quint8 *data, const QString &var) const
{
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
}
*/
