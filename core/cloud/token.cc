// core/cloud/token.cc
// 8/7/2011

#include "core/cloud/token.h"
#include "core/util/crypt.h"
#ifdef USE_WIN_DISK
  #include "win/disk/disk.h"
#endif // USE_WIN_DISK
#include <QtCore>

#define DEBUG "Core::Token"
#include "module/debug/debug.h"

#define DIGEST(_bytes)  Core::Crypt::md5(_bytes)
enum { DIGEST_SIZE = 10 * 1024 * 1024 }; // 10 MB

using namespace Core::Cloud;

// - Meta type -

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<Token>("Token");
    qRegisterMetaType<TokenList>("TokenList");
  } };
  init_ static_init_;
} // anonymous namespace

// - Digest -

QString
Core::Cloud::
Token::digestFromFile(const QString &input)
{
  DOUT("digestFromFile: enter:" << input);

  QByteArray data;
  QString filePath = input;
  QFileInfo fi(input);
  if (fi.isDir()) {
    QString dir = fi.filePath();
    filePath = dir + "/VIDEO_TS/VIDEO_TS.IFO";
    if (!QFileInfo(filePath).exists())
      filePath = dir + "/VIDEO_TS/VIDEO_TS.BUP";
    if (!QFileInfo(filePath).exists())
      filePath.clear();
  }

#ifdef USE_WIN_DISK
  if (filePath.isEmpty()) {
    QString deviceFileName = Disk::guessDeviceFileName(input);
    if (Disk::isValidDeviceFileName(deviceFileName)) {
      Disk disk(deviceFileName);
      bool succeed = disk.open(QIODevice::ReadOnly);
      if (!succeed) {
        DOUT("digestFromFile: Failed to open device disk for hashing: " << input);
        return QByteArray();
      }
      data = disk.read(DIGEST_SIZE);
      disk.close();
    }
  } else
#endif // USE_WIN_DISK
  if (!filePath.isEmpty()) {
    QFile file(filePath);
    bool succeed = file.open(QIODevice::ReadOnly);
    if (!succeed) {
      DOUT("digestFromFile: Failed to open file for hashing: " << filePath);
      return QByteArray();
    }

    if (file.size() <= DIGEST_SIZE)
      data = file.readAll();
    else
      data = file.read(DIGEST_SIZE);
    file.close();
  }

  if (data.isEmpty()) {
    DOUT("digestFromFile: Error: get empty data to hash: " << filePath);
    return QByteArray();
  }

  QByteArray ret = DIGEST(data);
  DOUT("digestFromFile: exit");
  return ret.toHex().toUpper();
}

// EOF
