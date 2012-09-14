#ifndef ROMAJITRANSLATOR_H
#define ROMAJITRANSLATOR_H

// romajitranslator.h
// 7/15/2012

#include "lib/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QTextEncoder)
QT_FORWARD_DECLARE_CLASS(QTextDecoder)

class RomajiTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(RomajiTranslator)
  typedef RomajiTranslator Self;
  typedef NetworkTranslator Base;

  QTextDecoder *decoder_;
  QTextEncoder *encoder_;

  // - Constructions -
public:
  explicit RomajiTranslator(QObject *parent = nullptr);

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, int to, int from) override
  { Q_UNUSED(to) Q_UNUSED(from) return createReply(text); }

  QNetworkReply *createReply(const QString &text);

  QString parseReply(const QByteArray &data) override;

protected:
  QString translateUrl(const QString &text) const;

  QString decodeText(const QByteArray &data) const;
  QByteArray encodeText(const QString &text) const;
};

#endif // ROMAJITRANSLATOR_H
