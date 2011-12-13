// core/cloud/token.cc
// 8/7/2011

#include "core/cloud/token.h"
#include "core/util/crypt.h"
#ifdef USE_MODULE_IOUTIL
  #include "module/ioutil/ioutil.h"
#endif // USE_MODULE_IOUTIL
#include <QtCore>
#include <memory>

#define DEBUG "core::token"
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
  DOUT("enter:" << input);

  QByteArray data;
  QString filePath = input;
  QFileInfo fi(input);
  if (fi.isDir()) {
    QString dir = fi.filePath();
    filePath = dir + "/VIDEO_TS/VIDEO_TS.IFO";
    if (!QFileInfo(filePath).exists())
      filePath = dir + "/VIDEO_TS/VIDEO_TS.BUP";
    if (!QFileInfo(filePath).exists())
      filePath = input;
  }

  if (!filePath.isEmpty()) {
#ifdef USE_MODULE_BLOCKIODEVICE
    data = IOUtil::readBytes(filePath, DIGEST_SIZE);
#else
    QFile file(filePath);
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok) {
      DOUT("exit: Failed to open file for hashing: " << filePath);
      return QByteArray();
    }

    data = file.read(DIGEST_SIZE);
    file.close();
#endif // USE_MODULE_BLOCKIODEVICE
  }

  if (data.isEmpty()) {
    DOUT("exit: Error: get empty data to hash: " << filePath);
    return QByteArray();
  }

  QByteArray ret = DIGEST(data);
  DOUT("exit");
  return ret.toHex().toUpper();
}

// EOF
