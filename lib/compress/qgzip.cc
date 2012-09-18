// qgzip.cc
// 2/3/2012

#include "lib/compress/qgzip.h"
#ifdef WITH_GZIP
# include <zlib.h>
#else
# error "require lib gzip"
#endif // WITH_GZIP

//#define DEBUG "qgzip"
#include "qtx/qxdebug.h"

// See: http://zlib.net/zpipe.c
// See: http://www.qtcentre.org/threads/30031-qUncompress-data-from-gzip
// See: http://stackoverflow.com/questions/2690328/qt-quncompress-gzip-data
QByteArray
gUncompress(const QByteArray &input, int zWindowBits)
{
  DOUT("enter: input.size =" << input.size());
  if (input.isEmpty()) {
    DOUT("exit: empty input");
    return QByteArray();
  }

  z_stream strm;
  qMemSet(&strm, 0, sizeof(z_stream));
  int status = zWindowBits ? ::inflateInit2(&strm, zWindowBits)
                           : ::inflateInit(&strm);
  if (status != Z_OK) {
    Q_ASSERT(0);
    DOUT("exit: zlib stream error");
    return QByteArray();
  }

  const char *start = input.constData();
  int size = input.size();
  // Strip header and trailer
  //input.remove(0, 10);
  //input.chop(12);
  //start += 10;
  //size -= 12;

  enum { bufsize = 1024 * 16 }; // 16K
  quint8 buf[bufsize];
  //if (!buf) {
  //  DOUT("exit: insufficent memory");
  //  return QByteArray();
  //}

  strm.next_in = (uchar *)start;
  strm.avail_in = size;

  QByteArray ret;
  do {
    strm.next_out = buf;
    strm.avail_out = bufsize;

    status = ::inflate(&strm, Z_SYNC_FLUSH);
    switch (status) {
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      ::inflateEnd(&strm);
      DOUT("exit: inflate error: status =" << status);
      return QByteArray();

    case Z_OK:
      Q_ASSERT(strm.avail_out == 0);
    case Z_STREAM_END:
      ret.append(QByteArray::fromRawData((char *)buf, bufsize - strm.avail_out));
    }

  } while(strm.avail_out == 0 && status != Z_STREAM_END);

  ::inflateEnd(&strm);

  DOUT("exit: status =" << status << ", ret.size =" << ret.size());
  return ret;
}

// EOF
