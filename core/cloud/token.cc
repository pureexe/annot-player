// core/cloud/token.cc
// 8/7/2011

#include "core/cloud/token.h"
#include "core/util/crypt.h"
#ifdef Q_WS_WIN
  #include "win/disk/disk.h"
#endif // Q_WS_WIN
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
Token::digestFromFile(const QString &fileName)
{
  DOUT("digestFromFile: enter:" << fileName);

  QByteArray cloud;
#ifdef Q_WS_WIN
  QString deviceFileName = Disk::guessDeviceFileName(fileName);
  if (Disk::isValidDeviceFileName(deviceFileName)) {
    Disk disk(deviceFileName);
    bool succeed = disk.open(QIODevice::ReadOnly);
    if (!succeed) {
      DOUT("digestFromFile: Failed to open device disk for hashing: " << fileName);
      return QByteArray();
    }
    cloud = disk.read(DIGEST_SIZE);
    disk.close();
  }
  else
#endif // Q_WS_WIN
  {
    QFile file(fileName);
    bool succeed = file.open(QIODevice::ReadOnly);
    if (!succeed) {
      DOUT("digestFromFile: Failed to open file for hashing: " << fileName);
      return QByteArray();
    }

    if (file.size() <= DIGEST_SIZE)
      cloud = file.readAll();
    else
      cloud = file.read(DIGEST_SIZE);
    file.close();
  }

  if (cloud.isEmpty()) {
    DOUT("digestFromFile: Error: get empty cloud to hash: " << fileName);
    return QByteArray();
  }

  QByteArray ret = DIGEST(cloud);
  DOUT("digestFromFile: exit");
  return ret.toHex().toUpper();
}

// EOF
