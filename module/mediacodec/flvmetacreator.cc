// flvmetacreator.cc
// 3/16/2012
// See: http://hackipedia.org/File%20formats/SWF,%20Macromedia-Adobe%20Flash/Adobe%20Flash%209%20File%20Format%20Specification.pdf
// Adobe Flash 9 File Format Specification.pdf (2007), The FLV Header (page 268)

#include "flvmeta.h"

#define DEBUG "flvmetacreator"
#include "module/debug/debug.h"

#define CHECK_OK        if (!ok) return false

// - Creator -

void
FlvMetaCreator::run()
{
  if (!parse())
    emit error(tr("failed to parse FLV streams"));
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
FlvMetaCreator::parse()
{
  DOUT("enter");
  meta_.clear();

  Q_ASSERT(is_);
  if (!in_)
    return false;

  state_ = Running;

  enum { HeaderSize = // 0x9
    + 4 // FLV tag
    + 1 // flags
    + 4 // data offset
  };
  //QByteArray header(HeaderSize, 0);
  //if (in_->read(header) != HeaderSize)
  //  return false;

  bool ok;
  if (in_->readUInt32(&ok) != 0x464C5601 || !ok) {
    DOUT("exit: ERROR: this isn't a FLV file.");
    return false;
  }

  quint32 flags = in_->readUInt8(&ok); CHECK_OK;
  Q_UNUSED(flags); // supposed to be 0x5 for FLV with both a/v tracks
  DOUT("FLV flags =" << flags);

  qint64 dataOffset = in_->readUInt32(&ok); CHECK_OK;
  if (!dataOffset) {
    DOUT("exit: malformed FLV with zero dataOffset");
    return false;
  }

  qint64 paddingSize = dataOffset - HeaderSize; // Often 0
  if (paddingSize > 0 &&
      in_->skip(paddingSize) != paddingSize)
    return false;

  quint32 prevTagSize = in_->readUInt32(&ok); CHECK_OK;
  if (prevTagSize)
    DOUT("warning: invalid FLV with non-zero first tag size:" << prevTagSize);

  while (ok && isRunning() && readTag())
    prevTagSize = in_->readUInt32(&ok);

  Q_ASSERT(in_->atEnd());

  meta_.fileSize = in_->size();
  meta_.duration = meta_.lastTimestamp;

  ok = isRunning();
  if (ok)
    state_ = Finished;

  DOUT("exit: ret =" << ok);
  return ok;
}

bool
FlvMetaCreator::readTag()
{
  enum TagType { AudioTag = 8, VideoTag = 9, ScriptTag = 18 };

  enum { TagSize = // 11 for FLV v1
    + 1 // tagType
    + 3 // dataSize
    + 4 // timestamp
    + 3 // streamId
  };
  enum { PreviousTagSize = sizeof(quint32) }; // 4

  //QByteArray tagData(TagSize, 0); // header
  //if (in_->read(tagData) != TagSize)
  //  return false;
  //DataInputStream tagIn(tagData);

  // Read tag header
  bool ok;
  quint32 tagType = in_->readUInt8(&ok); CHECK_OK;
  quint32 dataSize = in_->readUInt24(&ok); CHECK_OK;
  quint32 timestamp = in_->readUInt24(&ok); CHECK_OK;
  timestamp |= in_->readUInt8(&ok) << 24; CHECK_OK;
  quint32 streamId = in_->readUInt24(&ok); CHECK_OK;
  if (streamId)
    DOUT("warning: malformed FLV with non-zero streamId");

  meta_.lastTimestamp = timestamp;

  if (tagType != ScriptTag)
    meta_.dataSize += dataSize + TagSize + PreviousTagSize;

  switch (tagType) {
  case AudioTag:
    meta_.audioSize += dataSize + TagSize;
    break;
  case VideoTag:
    meta_.videoSize += dataSize + TagSize;
    break;
  }

  ok = in_->skip(dataSize) == dataSize;
  CHECK_OK;

  return true;
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
    Bitwise::BigEndian::getBytes(bytes, value);
    ::memcpy(data, bytes, size);
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
