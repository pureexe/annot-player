#ifndef TEXTCODECMANAGER_H
#define TEXTCODECMANAGER_H

// textcodecmanager.h
// 6/12/2012

#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QTextCodec)
QT_FORWARD_DECLARE_CLASS(QTextDecoder)

#define TEXT_CODEC_DEFAULT  "(System)"
#define TEXT_CODEC_JAPANESE "SHIFT-JIS"

class TextCodecManager : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(TextCodecManager)
  typedef TextCodecManager Self;
  typedef QObject Base;

  QTextCodec *codec_;
  QTextDecoder *decoder_;

  QString encoding_;

  // - Constructions -
public:
  static Self *globalInstance() { static Self g; return &g; }
public:
  explicit TextCodecManager(QObject *parent = nullptr);

  QTextCodec *codec() const { return codec_; }
  QTextDecoder *decoder() const { return decoder_; }

  QString encoding() const { return encoding_; }
signals:
  void encodingChanged(const QString &value);
public slots:
  void setEncoding(const QString &enc);

public:
  QString decode(const QByteArray &data) const;
};

#endif // TEXTCODECMANAGER_H
