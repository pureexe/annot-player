// disk.cc
// 8/17/2011
// CD-ROM: http://support.microsoft.com/kb/q138434/

#include "disk.h"
#include <windows.h>
#include <QtCore>
#include <cstring>
#include <limits>
#include <memory>

//#define DEBUG

#ifdef DEBUG
  #define DOUT(_msg)    qDebug() << "Disk:" << _msg
#else
  #define DOUT(_msg)    ;
#endif // DEBUG

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
  if (hint.isNull())
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
{
  return !fileName.isNull()
      && fileName.contains(QRegExp("^\\\\\\\\\\.\\\\[A-Z]:$"));
}

Disk::Geometry
Disk::geometryForHandle(void *handle)
{
  DOUT("geometryForHandle: enter");

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
        (LPOVERLAPPED)0 ); // synchronous I/O

  if (bResult) {
    Q_ASSERT(dwBytesReturned == sizeof(dgDisk));

    DOUT("geometryForHandle: succeeded");

    ret.mediaType = (Disk::MediaType)dgDisk.MediaType;
    ret.cylinders = dgDisk.Cylinders.QuadPart;
    ret.tracksPerCylinder = dgDisk.TracksPerCylinder;
    ret.sectorsPerTrack = dgDisk.SectorsPerTrack;
    ret.bytesPerSector = dgDisk.BytesPerSector;

  } else
    DOUT("geometryForHandle: failed with error:" << ::GetLastError());

  DOUT("geometryForHandle: exit");
  return ret;
}

// - Construction -

Disk::Disk(QObject *parent)
  : Base(parent), handle_(0)
{
  DOUT("Disk: enter");
  DOUT("fileName: " + fileName_);
  DOUT("Disk: exit");
}

Disk::Disk(const QString &fileName, QObject *parent)
  : Base(parent), fileName_(fileName), handle_(0)
{
  DOUT("Disk: enter");
  DOUT("fileName:" << fileName_);
  DOUT("Disk: exit");
}

QString
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
Disk::open(OpenMode mode)
{
  DOUT("open: enter");
  if (handle_) {
    handle_ = 0;
    Q_ASSERT(0);
    return false;
  }

  if (mode != QIODevice::ReadOnly) {
    Q_ASSERT(0);
    DOUT("open: exit: Invalid OpenMode");
    return false;
  }

  Q_ASSERT(isValidDeviceFileName(fileName_));

  HANDLE hDisk = ::CreateFileA(
        fileName_.toAscii(), // device file name
        GENERIC_READ, // Read-only
        FILE_SHARE_READ, // Read-only
        0, // Default security attributes
        OPEN_EXISTING,
        0, // File attribute
        0); // Template file

  if (hDisk != INVALID_HANDLE_VALUE) {
    DOUT("open: succeeded");

    handle_ = hDisk;
    geometry_ = geometryForHandle(handle_);

    Q_ASSERT(!geometry_.isNull());
    DOUT("Media type:" << geometry_.mediaType);
    DOUT("Media size:" << geometry_.size());
  } else
    DOUT("open: failed with error:" << ::GetLastError());

  DOUT("open: exit");
  return handle_;
}

void
Disk::close()
{
  DOUT("close: enter");
  if (!handle_) {
    Q_ASSERT(0);
    DOUT("close: exit");
    return;
  }

  Q_ASSERT(handle_ != INVALID_HANDLE_VALUE);
  if (handle_ != INVALID_HANDLE_VALUE) {
    BOOL bResult = ::CloseHandle((HANDLE)handle_);
    Q_ASSERT(bResult);
  }

  handle_ = 0;
  DOUT("close: exit");
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
  DOUT("read: enter: maxSize =" << maxSize);
  if (maxSize <= 0) {
    DOUT("read: exit");
    return QByteArray();
  }

  if (!handle_ || geometry_.isNull()
      || !geometry_.bytesPerSector) {
    DOUT("read: Invalid handle or geometry");
    Q_ASSERT(0);
    DOUT("read: exit");
    return QByteArray();
  }

  HANDLE hDisk = (HANDLE)handle_;
  Q_ASSERT(hDisk != INVALID_HANDLE_VALUE);

  DWORD dwReturnBytes = (DWORD)qMin((qint64)maxSize, size());
  DWORD dwBufferBytes = (dwReturnBytes / geometry_.bytesPerSector + 1) * geometry_.bytesPerSector;
  Q_ASSERT(dwBufferBytes >= dwReturnBytes);
  std::auto_ptr<char> buffer(new char[dwBufferBytes]);

  DWORD dwReadBytes;
  BOOL bResult = ::ReadFile(hDisk, buffer.get(), dwBufferBytes, &dwReadBytes, (LPOVERLAPPED)0); // read sector
  if (!bResult) {
    DOUT("read: failed to read file with error:" << ::GetLastError());
    DOUT("read: exit");
    return QByteArray();
  }
  DOUT("read: succeeded");
  DOUT("dwReturnBytes =" << dwReturnBytes);
  DOUT("dwBufferBytes =" << dwBufferBytes);
  DOUT("dwReadBytes =" << dwReadBytes);

  if (dwReadBytes < dwReturnBytes)
    dwReturnBytes = dwReadBytes;
  else if (dwReadBytes > dwReturnBytes) {
    DOUT("warning: inefficient read since specified bytes is not aligned to bytesPerSector");
    seek(dwReturnBytes);
  }

  DOUT("read: exit");
  return QByteArray(buffer.get(), (int)dwReturnBytes);
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

