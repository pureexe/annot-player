// textcodecmanager.cc
// 6/12/2012

#include "textcodecmanager.h"
#include <QtCore>

#define DEBUG "textcodecmanager"
#include "module/debug/debug.h"

// - Construction -

TextCodecManager::TextCodecManager(QObject *parent)
  : Base(parent), codec_(0), decoder_(0), encoding_(TEXT_CODEC_DEFAULT)
{
#ifdef Q_WS_WIN
  setEncoding(TEXT_CODEC_JAPANESE); // Enforce Japanese encoding
#endif // Q_WS_WIN
}

// - Properties -

void
TextCodecManager::setEncoding(const QString &enc)
{
  if (encoding_.compare(enc, Qt::CaseInsensitive)) {
    encoding_ = enc;
    DOUT("encoding =" << encoding_);
    if (encoding_ == TEXT_CODEC_DEFAULT) {
      decoder_ = 0;
      codec_ = 0;
    } else {
      codec_ = QTextCodec::codecForName(encoding_.toLocal8Bit());
      decoder_ = codec_ ? codec_->makeDecoder() : 0;
    }
    emit encodingChanged(encoding_);
  }
}

// - Actions -

QString
TextCodecManager::decode(const QByteArray &data) const
{ return decoder_ ? decoder_->toUnicode(data) : QString::fromLocal8Bit(data); }

// EOF
