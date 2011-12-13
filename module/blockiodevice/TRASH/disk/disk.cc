// disk.cc
// 8/17/2011
// CD-ROM:
// CD geometry: http://www.isobuster.com/help.php?help=190
// - Raw: 2352
// - Track: 2048 (Audio) or others (data)
// See: http://www.codeproject.com/KB/audio-video/SimpleAudioCD.aspx
// See: http://social.msdn.microsoft.com/Forums/en-us/vssmartdevicesnative/thread/3c5daa34-afae-4a4a-a685-e282983df783

#include "disk.h"
#include <qt_windows.h>
#include <winioctl.h>
#include <ntddcdrm.h>
#include <QtCore>
#include <climits>
#include <cstring>
#include <memory>

#define DEBUG "disk"
#include "module/debug/debug.h"

namespace { // anonymous
  enum { CD_RAW_READ_SIZE = 2352 };
} // anonymous namespace

// - Geometry -

bool
Disk::Geometry::isNull() const
{ return !mediaType && !cylinders && !tracksPerCylinder && !sectorsPerTrack && !bytesPerSector; }

void
Disk::Geometry::clear()
{ ::memset(this, 0, sizeof(*this)); }

// - Helpers -

QString
Disk::guessDeviceFileName(const QString &hint)
{
  if (hint.isEmpty())
    return QString();

  QString normalized = hint.trimmed().toUpper();
  if (normalized.isEmpty())
    return QString();

  else if (normalized.contains(QRegExp("^\\\\\\\\\\.\\\\[A-Z]:$")))
    return normalized;
  else if (normalized.contains(QRegExp("^[A-Z]:$")))
    return "\\\\.\\" + normalized;
  else if (normalized.contains(QRegExp("^[A-Z]:\\\\$"))) {
    normalized.chop(1);
    return "\\\\.\\" + normalized;
  }
  else
    return QString();
}

bool
Disk::isValidDeviceFileName(const QString &fileName)
{ return fileName.contains(QRegExp("^\\\\\\\\\\.\\\\[A-Z]:$", Qt::CaseInsensitive)); }

Disk::Geometry
Disk::geometryForHandle(void *handle)
{
  DOUT("enter");

  // DISK_GEOMETRY: http://social.msdn.microsoft.com/Forums/en/vclanguage/thread/92a092c8-87a9-4fb8-8d25-e6d15d4266ed
  // http://support.microsoft.com/kb/q138434/
  Geometry ret;
  Q_ASSERT(handle);
  if (!handle)
    return ret;

  // DeviceIoControl: http://msdn.microsoft.com/en-us/library/aa363216(v=vs.85).aspx
  DISK_GEOMETRY dgDisk;
  DWORD dwBytesReturned;
  HANDLE hDevice = (HANDLE)handle;
  BOOL bResult = ::DeviceIoControl(
    hDevice, // device to be queried
    IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
    0, 0, // no input buffer
    &dgDisk, sizeof(dgDisk), // output buffer
    &dwBytesReturned, // # bytes returned
    (LPOVERLAPPED)0 // synchronous I/O
  );

  if (bResult) {
    Q_ASSERT(dwBytesReturned == sizeof(dgDisk));

    DOUT("succeeded");

    ret.mediaType = (Disk::MediaType)dgDisk.MediaType;
    ret.cylinders = dgDisk.Cylinders.QuadPart;
    ret.tracksPerCylinder = dgDisk.TracksPerCylinder;
    ret.sectorsPerTrack = dgDisk.SectorsPerTrack;
    ret.bytesPerSector = dgDisk.BytesPerSector;

  } else
    DOUT("failed with error:" << ::GetLastError());

  DOUT("exit");
  return ret;
}

// - Construction -

Disk::~Disk()
{
  Q_ASSERT(!handle_);
  Q_ASSERT(!pmr_);
}

Disk::Disk(QObject *parent)
  : Base(parent), handle_(0), pmr_(0)
{
  DOUT("enter");
  DOUT("fileName: " + fileName_);
  DOUT("exit");
}

Disk::Disk(const QString &fileName, QObject *parent)
  : Base(parent), fileName_(fileName), handle_(0), pmr_(0)
{
  DOUT("enter");
  DOUT("fileName:" << fileName_);
  DOUT("exit");
}

const QString &
Disk::fileName() const
{ return fileName_; }

void
Disk::setFileName(const QString &fileName)
{ fileName_ = fileName; }

Disk::Geometry
Disk::geometry() const
{ return geometry_; }

void
Disk::setGeometry(const Geometry &geo)
{ geometry_ = geo; }

qint64
Disk::size() const
{
  Q_ASSERT(!geometry_.isNull());
  return geometry_.size();
}

// - I/O -

bool
Disk::isOpen() const
{ return handle_; }

bool
Disk::open(OpenMode mode, bool lock)
{
  DOUT("enter");
  if (handle_) {
    handle_ = 0;
    Q_ASSERT(0);
    return false;
  }

  if (mode != QIODevice::ReadOnly) {
    Q_ASSERT(0);
    DOUT("exit: Invalid OpenMode");
    return false;
  }

  Q_ASSERT(isValidDeviceFileName(fileName_));

  HANDLE hDisk = ::CreateFileA(
    fileName_.toAscii(), // device file name
    GENERIC_READ, // Read-only
    FILE_SHARE_READ, // Read-only
    0, // Default security attributes
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, // File attribute
    0 // Template file
  );

  if (hDisk != INVALID_HANDLE_VALUE) {
    DOUT("succeeded");

    if (lock)
      setRemovableMediaLocked(lock);

    handle_ = hDisk;
    geometry_ = geometryForHandle(handle_);

    Q_ASSERT(!geometry_.isNull());
    DOUT("media type =" << geometry_.mediaType);
    DOUT("open media size =" << geometry_.size());
  } else
    DOUT("failed with error:" << ::GetLastError());

  DOUT("exit");
  return handle_;
}

void
Disk::close()
{
  DOUT("enter");
  if (!handle_) {
    Q_ASSERT(0);
    DOUT("exit");
    return;
  }

  Q_ASSERT(handle_ != INVALID_HANDLE_VALUE);
  if (handle_ != INVALID_HANDLE_VALUE) {
    if (isRemovableMediaLocked())
      setRemovableMediaLocked(false);
    BOOL bResult = ::CloseHandle((HANDLE)handle_);
    Q_ASSERT(bResult);
    Q_UNUSED(bResult);
  }

  handle_ = 0;
  DOUT("exit");
}

// See: http://www.cplusplus.com/forum/windows/30402/
qint64
Disk::pos() const
{
  Q_ASSERT(isOpen());
  if (geometry_.size() <= (qint64)4 * 1024 * 1024 * 1024) { // 4GB
    // FIXME: THE ERROR IS NOT CHECKED!
    return ::SetFilePointer((HANDLE)handle_, 0, 0, FILE_CURRENT);

  } else {
    LARGE_INTEGER liRet,
                  liPos = { {0, 0} };

    BOOL bResult = ::SetFilePointerEx((HANDLE)handle_, liPos, &liRet, FILE_CURRENT);
    if (bResult)
      return liRet.QuadPart;
    else
      return -1;
  }
}

bool
Disk::seek(qint64 pos)
{
  Q_ASSERT(isOpen());
  if (geometry_.size() <= (qint64)4 * 1024 * 1024 * 1024) { // 4GB
    LARGE_INTEGER liPos;
    liPos.QuadPart = pos;

    if (liPos.u.HighPart)
      ::SetFilePointer((HANDLE)handle_, liPos.u.LowPart, &liPos.u.HighPart, FILE_BEGIN);
    else
      ::SetFilePointer((HANDLE)handle_, pos, 0, FILE_BEGIN);

    // FIXME: THE ERROR IS NOT CHECKED!
    return true;

  } else {
    LARGE_INTEGER liPos;
    liPos.QuadPart = pos;

    return ::SetFilePointerEx((HANDLE)handle_, liPos, 0, FILE_BEGIN);
  }
}

QByteArray
Disk::read(int maxSize)
{
  DOUT("enter: maxSize =" << maxSize);
  if (maxSize <= 0) {
    DOUT("exit");
    return QByteArray();
  }

  if (!handle_ || geometry_.isNull()
      || !geometry_.bytesPerSector) {
    DOUT("Invalid handle or geometry");
    Q_ASSERT(0);
    DOUT("exit");
    return QByteArray();
  }

  HANDLE hDisk = (HANDLE)handle_;
  Q_ASSERT(hDisk != INVALID_HANDLE_VALUE);

  DWORD dwReturnBytes = qMin((qint64)maxSize, size());
  DWORD dwBufferBytes = dwReturnBytes;
  DWORD dwBlockSize = geometry_.bytesPerSector;
  if (dwBufferBytes % dwBlockSize)
      dwBufferBytes = (dwBufferBytes / dwBlockSize + 1) * dwBlockSize;
  DOUT(" dwReturnBytes =" << dwReturnBytes <<
       ", dwBufferBytes =" << dwBufferBytes <<
       ", dwBlockSize =" << dwBlockSize);
  Q_ASSERT(dwBufferBytes >= dwReturnBytes);
  std::auto_ptr<char> buffer(new char[dwBufferBytes]);

  int length = dwBufferBytes / CD_RAW_READ_SIZE;
  char* pBuf = new char [dwBufferBytes];
  RAW_READ_INFO ReadInfo;
  ReadInfo.TrackMode = CDDA; // Always use CDDA (numerical: 2)

  ReadInfo.SectorCount = 20; // We'll read 20 sectors with each operation.

  // Read the track-data in a loop. Read 20*2352 bytes per pass.
  DWORD a;
  CDROM_DISK_DATA cdd;
  qDebug()<<44444<<DeviceIoControl(hDisk, IOCTL_CDROM_DISK_TYPE, 0, 0, &cdd, sizeof(cdd), &a, 0)
            << cdd.DiskData << CDROM_DISK_AUDIO_TRACK;
  ULONG i;
  for (i=0; i<length / 20; i++)
  {
    // Calculate the new offset from where to read.
    ReadInfo.DiskOffset.QuadPart = (0 + i*20) * 2048;
    // Call DeviceIoControl and read the audio-data to out buffer.
    ULONG Dummy;
    if (0 == DeviceIoControl( hDisk, IOCTL_CDROM_RAW_READ,
                              &ReadInfo, sizeof(ReadInfo),
                              pBuf+i*20*2352,
                                 20*2352,
                                 &Dummy, NULL ) )
    {
    qDebug()<<"111111";
    }

  }
  //delete [] pBuf;

  // Read the remaining sectors.

  ReadInfo.SectorCount = length % 20;
  ReadInfo.DiskOffset.QuadPart = (0+ i*20) * 2048;
  ULONG Dummy;
  if ( 0 == DeviceIoControl( hDisk, IOCTL_CDROM_RAW_READ,
                             &ReadInfo, sizeof(ReadInfo),
                             pBuf+i*20*2352,
                             ReadInfo.SectorCount*2352,
                             &Dummy, NULL ) )
  {
    qDebug()<<"222222";
  }

    qDebug()<<"3333333";
  delete [] pBuf;

  DWORD dwReadBytes;
  BOOL bResult = ::ReadFile(hDisk, buffer.get(), dwBufferBytes, &dwReadBytes, (LPOVERLAPPED)0); // read sector
  DOUT("bResult = " << bResult << ", dwReadBytes =" << dwReadBytes);
  if (!bResult || !dwReadBytes) {
    DOUT("failed to read file with error:" << ::GetLastError());
    DOUT("exit");
    return QByteArray();
  }
  DOUT("succeeded");

  if (dwReadBytes < dwReturnBytes)
    dwReturnBytes = dwReadBytes;
  else if (dwReadBytes > dwReturnBytes) {
    DOUT("warning: inefficient read since specified bytes is not aligned to bytesPerSector");
    seek(dwReturnBytes);
  }

  DOUT("exit");
  return QByteArray(buffer.get(), dwReturnBytes);
}

// - Lock/unlock removable media -

bool
Disk::isRemovableMediaLocked() const
{ return pmr_; }

void
Disk::setRemovableMediaLocked(bool t)
{
  DOUT("enter: toLock =" << t);
  if (t == isRemovableMediaLocked()) {
    DOUT("exit: no changes");
    return;
  }

  HANDLE hDisk = (HANDLE)handle_;
  if (hDisk == INVALID_HANDLE_VALUE) {
    DOUT("exit: invalid disk handle");
    return;
  }

  PREVENT_MEDIA_REMOVAL *pmr = (PREVENT_MEDIA_REMOVAL*)pmr_; // type-cast
  if (!pmr)
    pmr = new PREVENT_MEDIA_REMOVAL;
  pmr->PreventMediaRemoval = t ? TRUE : FALSE; // TRUE: lock, FALSE: unlock

  DWORD dwBytesReturned; // unused
  BOOL bResult = ::DeviceIoControl(
    (HANDLE)hDisk,
    IOCTL_CDROM_MEDIA_REMOVAL,
    pmr, sizeof(*pmr),
    0, 0, &dwBytesReturned, 0
  );

  if (!t || !bResult) { // if unlock or failed to lock, delete pmr
    delete pmr;
    pmr = 0;
  }

  pmr_ = pmr;

  DOUT("exit: locked =" << (bool)pmr_);
}

// EOF

/*
// See: http://lavica.fesb.hr/cgi-bin/info2html?(libcdio)Example%201
#ifdef Q_WS_WIN
  // Needed by <cdio/cdio.h>
  typedef size_t ssize_t;
  #include <inttypes.h>
#endif // Q_WS_WIN
#include <cdio/cdio.h>
*/

