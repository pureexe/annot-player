// blockiodevice.cc
// 12/3/2011
//
// CD geometry: http://www.isobuster.com/help.php?help=190
//
// Linux kernel
// See: http://www.mjmwired.net/kernel/Documentation/ioctl/cdrom.txt
// See: http://www.mjmwired.net/kernel/Documentation/ioctl/hdio.txt
//
// Windows DDK:
// See: http://www.codeproject.com/KB/audio-video/SimpleAudioCD.aspx
// See: http://social.msdn.microsoft.com/Forums/en-us/vssmartdevicesnative/thread/3c5daa34-afae-4a4a-a685-e282983df783

#include "blockiodevice.h"
extern "C" {
#ifdef Q_OS_WIN
#  include <qt_windows.h>
#  include <winioctl.h>
#  include <ntddcdrm.h>
#endif // Q_OS_WIN
#ifdef Q_OS_UNIX
#  ifdef Q_OS_LINUX
//#    include <linux/hdreg.h>
#    include <linux/cdrom.h>
#  elif defined Q_OS_MAC
#    include <sys/disk.h>
#  endif // Q_OS_
#  include <sys/ioctl.h>
#  include <fcntl.h>
#  include <cstring>
#  include <cerrno>
#endif // Q_OS_UNIX
} // extern "C"

#define DEBUG "blockiodevice"
#include "module/debug/debug.h"

#ifdef Q_OS_MAC
  // See: http://www.filibeto.org/unix/macos/lib/dev/documentation/DeviceDrivers/Conceptual/WorkingWStorage/WorkingWStorage.pdf
  #define IOCTL_BLOCKSIZE DKIOCGETBLOCKSIZE
#elif defined(Q_OS_LINUX)
  // See: http://lists.trolltech.com/qt-interest/2005-03/msg00270.html
  // See: http://wolfie.stfu.cz/src/blockdev.c
  #define IOCTL_DEVICESIZE  _IO(0x12,96)      // get partition/device size
  #define IOCTL_SECTORS_PER_REQ _IO(0x12,103) // get max sectors per request
  #define IOCTL_SECTORSIZE  _IO(0x12,104)     // get sector size, 2048 for CD-ROM
  #define IOCTL_BLOCKSIZE   IOCTL_SECTORSIZE

  namespace { // anonymous
    union cdrom_read_raw {
      cdrom_msf msf;                    // input address
      char buffer[CD_FRAMESIZE_RAW];    // return data
    };
  } // anonymous
#endif // Q_OS_

#ifndef CD_FRAMESIZE_RAW
  //#define CD_FRAMESIZE_AUDIO    2048
  #define CD_FRAMESIZE_RAW      2352
#endif // Q_OS_LINUX


// - Helpers -

namespace { // anonymous, CD access

  typedef BlockIODevice::fd_t fd_t;

  inline bool isValidFd_(fd_t fd)
  {
#ifdef Q_OS_WIN
    return fd != INVALID_HANDLE_VALUE;
#else
    return fd >= 0;
#endif // Q_OS_WIN
  }

  inline bool isAudioCD_(fd_t fd)
  {
    bool ret = false;
#ifdef Q_OS_WIN
    DWORD dwReadSize; // not used
    CDROM_DISK_DATA cdd;
    ret = ::DeviceIoControl((HANDLE)fd, IOCTL_CDROM_DISK_TYPE,
                             nullptr, 0, // no input
                             &cdd, sizeof(cdd), &dwReadSize, // output
                             nullptr) // synchronized
        && cdd.DiskData == CDROM_DISK_AUDIO_TRACK;
#elif defined(Q_OS_LINUX)
    ret = ::ioctl(fd, CDROM_DISC_STATUS, 0) == CDS_AUDIO;
#elif defined (Q_OS_MAC)
    // FIXME: need a to figure out how to get disk type through ioctl on Mac OS X
    u_int32_t blockSize;
    if (::ioctl(fd, IOCTL_BLOCKSIZE, &blockSize) >= 0)
      ret = blockSize == CD_FRAMESIZE_RAW;
#else
    Q_UNUSED(fd);
#endif // Q_OS_
    return ret;
  }

  inline int getBlockSize_(fd_t fd)
  {
    Q_ASSERT(::isValidFd_(fd));
    uint ret;
    if (isAudioCD_(fd))
      ret = CD_FRAMESIZE_RAW; // 2352
    else
#ifdef Q_OS_WIN
    {
      DISK_GEOMETRY dgDisk;
      DWORD dwBytesReturned;
      BOOL bResult = ::DeviceIoControl(
        (HANDLE)fd, IOCTL_DISK_GET_DRIVE_GEOMETRY,
        nullptr, 0, // no input
        &dgDisk, sizeof(dgDisk), // output
        &dwBytesReturned, // # bytes returned
        nullptr // synchronous I/O
      );
      if (bResult)
        ret = dgDisk.SectorsPerTrack;
      else
        DOUT("DeviceIoControl error:" << ::GetLastError());
    }
#else // UNIX
  #ifdef IOCTL_BLOCKSIZE
    if (::ioctl(fd, IOCTL_BLOCKSIZE, &ret) < 0)
  #endif // IOCTL_BLOCKSIZE
    ret = 0;
#endif // Q_WS_WIN
    return (int)ret;
  }

} // anonymous namespace

// - Properties -

bool
BlockIODevice::isAudioCD() const
{ return ::isValidFd_(fd_ ) ? ::isAudioCD_(fd_) : false; }

// - Open/close -

bool
BlockIODevice::open(OpenMode mode)
{
  DOUT("open:enter: mode =" << mode << ", fileName_ =" << fileName_);
  Q_ASSERT(!isOpen() && !::isValidFd_(fd_));
  Q_ASSERT(mode == ReadOnly);
  if (mode != ReadOnly) {
    DOUT("open:exit: ret = false, exit from non ReadOnly path");
    return false;
  }

  Q_ASSERT(!fileName_.isEmpty());
#ifdef Q_OS_WIN
  fd_ = ::CreateFileA(
    fileName_.toLocal8Bit(), // device file name
    GENERIC_READ, // Read-only
    FILE_SHARE_READ, // Read-only
    nullptr, // Default security attributes
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, // File attribute
    nullptr // Template file
  );
#else
  fd_ = ::open(fileName_.toLocal8Bit(), O_RDONLY | O_NONBLOCK);
#endif // Q_OS_WIN
  if (::isValidFd_(fd_)) { // succeedded
    setOpenMode(mode);
    blockSize_ = ::getBlockSize_(fd_);
    if (!blockSize_)
      DOUT("open: WARNING: device is not blocked");
    else
      DOUT("open: blockSize_ =" << blockSize_);
  }
  DOUT("open:exit: fd_ =" << fd_);
  return isOpen();
}

void
BlockIODevice::close()
{
  DOUT("close:enter: isOpen =" << isOpen() << ", fd_ =" << fd_);
  Q_ASSERT(isOpen() && ::isValidFd_(fd_ ));
  if (::isValidFd_(fd_)) {
#ifdef Q_OS_WIN
    ::CloseHandle((HANDLE)fd_);
#else
    ::close(fd_);
#endif // Q_OS_WIN
    fd_ = (fd_t)-1;
    blockSize_ = 0;
  }
  DOUT("close:exit");
}

// - I/O -

qint64
BlockIODevice::readData(char *data, qint64 maxSize)
{
  Q_ASSERT(isOpen() && ::isValidFd_(fd_));
  Q_ASSERT(maxSize);
  DOUT("readData:enter: maxSize =" << maxSize);

  qint64 ret = 0;
  if (maxSize % blockSize_)
    DOUT("readData: WARNING: maxSize is not blocked");
  else
#ifdef Q_OS_WIN
  if (isAudioCD()) {
    DOUT("readData: take windows audio CD code path");
    // Largest possible number tested on win7. No idea if it is portable.
    enum { blocks_per_req = 55 };
    enum { req_size = blocks_per_req * CD_FRAMESIZE_RAW };
    qint64 blockCount = maxSize / blockSize_;

    HANDLE h = (HANDLE)fd_;

    RAW_READ_INFO rri; {
      rri.TrackMode = CDDA; // Always use CDDA (numerical: 2)
      rri.SectorCount = blocks_per_req; //read 20 sectors with each operation.
      rri.DiskOffset.QuadPart = 0;
    }

    BOOL bResult = TRUE;
    ULONG dwReadSize = req_size; // Number of bytes read per request
    char *pBuffer = data;
    for (qint64 i = 0; (i < blockCount / blocks_per_req) &&
                        bResult && dwReadSize == req_size; // not EOF
         i++) {
      // Calculate the new offset from where to read.
      // Call DeviceIoControl and read the audio-data to out buffer.
      bResult = ::DeviceIoControl(
        h, IOCTL_CDROM_RAW_READ,
        &rri, sizeof(rri),
        pBuffer, req_size,
        &dwReadSize, nullptr
      );
      if (bResult) {
        rri.DiskOffset.QuadPart += dwReadSize;
        pBuffer += dwReadSize;
      }
    }
    ret = rri.DiskOffset.QuadPart;

    // Read the remaining sectors.
    if (bResult && dwReadSize == req_size &&
        (rri.SectorCount = blockCount % blocks_per_req)) {
      bResult = ::DeviceIoControl(
        h, IOCTL_CDROM_RAW_READ,
        &rri, sizeof(rri),
        pBuffer, rri.SectorCount * CD_FRAMESIZE_RAW,
        &dwReadSize, nullptr
      );
      if (bResult)
        ret += dwReadSize;
    }
  } else {
    DOUT("readData: take windows block device code path");
    DWORD dwReadSize;
    BOOL bResult = ::ReadFile((HANDLE)fd_, data, maxSize, &dwReadSize, nullptr);
    if (bResult)
      ret = dwReadSize;
    else
      DOUT("readData: ReadFile error:" << ::GetLastError());
  }
#elif defined(Q_OS_UNIX)
#ifdef Q_OS_LINUX
  if (isAudioCD()) {
    DOUT("readData: take linux audio CD code path");
    qint64 blockCount = maxSize / blockSize_;
    enum { blocks_per_req = 20 }; // nframes, must in range [1 75]
    enum { req_size = blocks_per_req * CD_FRAMESIZE_RAW };

    cdrom_read_audio ra; {
      ra.addr.lba = 0;
      ra.addr_format = CDROM_LBA;
      ra.buf = (__u8*)data;
      ra.nframes = blocks_per_req;
    }

    //cdrom_msf msf = { };
    //msf.cdmsf_frame0 = 0;
    //msf.cdmsf_frame1 = 1;
    //cdrom_read_raw *p_rr = reinterpret_cast<cdrom_read_raw*>(data);
    //p_rr->msf = msf;
    //::ioctl(fd_, CDROMREADRAW, p_rr);

    bool ok = true;
    for (int i = 0; i < blockCount / blocks_per_req; i++) {
      ok = ::ioctl(fd_, CDROMREADAUDIO, &ra) >= 0;
      if (!ok)
        break;

      ra.addr.lba += req_size;
      ra.buf += req_size;
    }
    ret = ra.addr.lba;

    if (ok &&
        (ra.addr.lba = blockCount % blocks_per_req)) {
      ok = ::ioctl(fd_, CDROMREADAUDIO, &ra) >= 0;
      if (ok)
        ret += req_size;
    }

    // FIXME jichi 12/7/2011: Only first 120 frames could be read ... WHY?!
    if (!ok)
      DOUT("readData:error: failed to read audio CD: ioctl: " << ::strerror(errno));

  } else
#endif // Q_OS_LINUX
  {
    DOUT("readData: take unix block device code path");
    ret = ::read(fd_, data, maxSize);
  }
#endif // Q_OS_
  DOUT("readData:exit: ret =" << ret);
  return ret;
}

qint64
BlockIODevice::writeData(const char *data, qint64 maxSize)
{
  Q_ASSERT(isOpen() && ::isValidFd_(fd_));
  DOUT("writeData:enter: maxSize =" << maxSize);
  qint64 ret = 0;
#ifdef Q_OS_UNIX
  if (maxSize % blockSize_)
    DOUT("writeData: WARNING: maxSize is not blocked");
  else
    ret = ::write(fd_, data, maxSize);
#else
  Q_UNUSED(data);
  Q_UNUSED(maxSize);
  // Not implemented yet.
#endif // Q_OS_UNIX
  DOUT("writeData:exit: ret =" << ret);
  return ret;
}

// EOF

/*
int main(int argc, char * argv[])
{
    struct hd_geometry geom;
    unsigned long size = 0, secsize = 0;
    int fd;

    progname = argv[0];

    if(argc != 2)         usage();

       if((fd = open(argv[1], O_RDONLY)) < 0)
        error("Can't open device", -1);

    // get C/H/S geometry and size of partition
    if(ioctl(fd, HDIO_GETGEO, &geom) < 0)
        error("HDIO_GETGEO ioctl failed", fd);

    // get size of the partition, chould match C*H*S
    if(ioctl(fd, BLKGETSIZE, &size) < 0)
        error("BLKGETSIZE ioctl failed", fd);

    // get size of block
    if(ioctl(fd, BLKSSZGET, &secsize) < 0)
        error("BLKSSZGET ioctl failed", fd);

    printf("\n");
    printf("Geometry for %s:\n", argv[1]);
    printf("\t   Cylinders: %d\n", geom.cylinders);
    printf("\t       Heads: %d\n", geom.heads);
    printf("\t     Sectors: %d\n", geom.sectors);
    printf("\t Sector Size: %d\n", secsize);
    printf("\t     Sectors: %d\n", size);
    printf("\tFirst sector: %d\n", geom.start);
    printf("\t Last sector: %d\n", geom.start + size - 1);
    printf("\n");

    if(close(fd) == -1)
        error("close error", -1);

    return 0;

}
*/
