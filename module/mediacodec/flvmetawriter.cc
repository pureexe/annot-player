// flvmetawriter.cc
// 3/16/2012

#include "flvmeta.h"
#include "module/stream/datainputstream.h"
#include <QtCore>

#define DEBUG "flvmetawriter"
#include "module/debug/debug.h"

// - Writer -

void
FlvMetaWriter::run()
{
  if (!update())
    emit error(tr("failed to merge FLV streams"));
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
FlvMetaWriter::update()
{
  DOUT("enter");

  Q_ASSERT(ios_);
  if (!ios_)
    return false;

  state_ = Running;

  InputStream *in = ios_;

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

  qint64 paddingSize = dataOffset - HeaderSize; // Often 0
  if (paddingSize > 0) {
    if (in->skip(paddingSize) != paddingSize)
      return false;
  }

  quint32 prevTagSize = in->readUInt32(&ok);
  if (!ok)
    return false;
  if (prevTagSize)
    DOUT("warning: invalid FLV with non-zero first tag size:" << prevTagSize);

  ok = seekToScriptTag(in) && updateScriptTag() && isRunning();
  if (ok)
    finish();

  DOUT("exit: ret =" << ok);
  return ok;
}

void
FlvMetaWriter::finish()
{
  DOUT("enter");
  Q_ASSERT(out_);
  ios_->finish();
  state_ = Finished;
  DOUT("exit");
}

bool
FlvMetaWriter::seekToScriptTag(InputStream *in)
{
  Q_ASSERT(ios_);
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
  quint32 tagType = tagIn.readUInt8();
  quint32 dataSize = tagIn.readUInt24();
  //quint32 timestamp = tagIn.readUInt24();
  //timestamp |= tagIn.readUInt8() << 24;
  //quint32 streamId = tagIn.readUInt24();

  if (tagType != ScriptTag) {
    DOUT("exit: first tag is not a script tag");
    return false;
  }

  // Read tag data
  if (dataSize >= in->size()) // no enough data
    return false;

  if (in->skip(1) != 1) // skip first byte (mediaInfo)
    return false;
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
FlvMetaWriter::updateScriptTag()
{
#define CHECK_OK        if (!ok) return false
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
  Q_ASSERT(ios_);
  bool ok = true;
  InputStream *in = ios_;
  OutputStream *out = ios_;
  CHECK_OK;

  // - ScriptDataString
  // - ScriptDataValue
  quint16 stringLength = in->readUInt16(&ok); CHECK_OK;
  QByteArray stringData(stringLength, 0);
  ok = in->read(stringData) == stringLength; CHECK_OK;
  QString var(stringData);

  quint8 valueType = in->readUInt8(&ok); CHECK_OK;
  switch (valueType) {
  case DoubleType:
    if (!updateScriptTagDoubleValue(out, var)) {
      enum { size = sizeof(double) };
      ok = in->skip(size) == size;
      CHECK_OK;
    } break;
  case UInt8Type:
    if (!updateScriptTagUInt8Value(out, var)) {
      enum { size = sizeof(quint8) };
      ok = in->skip(size) == size;
      CHECK_OK;
    } break;
  case UInt16Type:
    {
      enum { size = sizeof(quint16) };
      ok = in->skip(size) == size;
      CHECK_OK;
    } break;
  case DateType:
    {
      enum { size = sizeof(quint64) + sizeof(quint16) };
      ok = in->skip(size) == size;
      CHECK_OK;
    } break;
  case StringType:
  case StringType_Path:
    {
      quint16 valueLength = in->readUInt16(&ok); CHECK_OK;
      ok = in->skip(valueLength) == valueLength; CHECK_OK;
    } break;
  case LongStringType:
    {
      quint32 valueLength = in->readUInt32(&ok); CHECK_OK;
      ok = in->skip(valueLength) == valueLength; CHECK_OK;
    } break;
  case ECMAArrayType:
    {
      quint32 valueCount = in->readUInt32(&ok); CHECK_OK;
      DOUT("meta:" << var << "array" << valueCount);
      for (quint32 i = 0; i < valueCount; i++) {
        ok = updateScriptTag();
        CHECK_OK;
      }
    }
  }
  return ok;
#undef CHECK_OK
}

bool
FlvMetaWriter::updateScriptTagDoubleValue(OutputStream *out, const QString &var) const
{
#define ELIF_VAR(_var, _meta) \
  else if (var == _var) { \
    if (meta_._meta) { \
      update = true; \
      value = meta_._meta; \
    } \
  }

  enum { size = sizeof(double) };
  bool update = false;
  double value;
  if (var == "duration") {
    if (meta_.duration) {
      update = true;
      value = meta_.duration / 1000.0;
    }
  } if (var == "lasttimestamp") {
    if (meta_.lastTimestamp) {
      update = true;
      value = meta_.lastTimestamp / 1000.0;
    }
  } if (var == "lastkeyframetimestamp") {
    if (meta_.lastKeyFrameTimestamp) {
      update = true;
      value = meta_.lastKeyFrameTimestamp / 1000.0;
    }
  } if (var == "filesize") {
    if (meta_.fileSize) {
      update = true;
      value = meta_.fileSize;
    }
  } ELIF_VAR("datasize", dataSize)
  ELIF_VAR("audiosize", audioSize)
  ELIF_VAR("videosize", videoSize)
  ELIF_VAR("lastkeyframelocation", lastKeyFrameLocation)

  if (update) {
    out->writeDouble(value);
    DOUT("update" << var << "=" << value);
  }
  return update;
#undef ELIF_VAR
}

bool
FlvMetaWriter::updateScriptTagUInt8Value(OutputStream *out, const QString &var) const
{
#define ELIF_VAR(_var, _meta) \
  else if (var == _var) { \
    update = true; \
    value = meta_._meta ? 1 : 0; \
  }

  bool update = false;
  quint8 value;

  if (var == "hasKeyframes") {
    update = true;
    value = meta_.hasKeyFrames() ? 1 : 0;
  }
  ELIF_VAR("hasMetadata", hasMetaData)
  ELIF_VAR("canSeekToEnd", canSeekToEnd)

  if (update) {
    out->writeUInt8(value);
    DOUT("update" << var << "=" << value);
  }
  return update;
#undef ELIF_VAR
}

// EOF
