#ifndef ROMAJITRANSLATOR_H
#define ROMAJITRANSLATOR_H

// romajitranslator.h
// 7/15/2012

#include "module/translator/networktranslator.h"

QT_BEGIN_NAMESPACE
class QNetworkReply;
class QTextEncoder;
class QTextDecoder;
QT_END_NAMESPACE

class RomajiTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(RomajiTranslator)
  typedef RomajiTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;
  QTextDecoder *decoder_;
  QTextEncoder *encoder_;

  // - Constructions -
public:
  explicit RomajiTranslator(QObject *parent = nullptr);

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override
  { Q_UNUSED(to) Q_UNUSED(from) translate(text); }

  void translate(const QString &text);

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  QString translateUrl(const QString &text) const;

  QString decodeText(const QByteArray &data) const;
  QByteArray encodeText(const QString &text) const;
};

#endif // ROMAJITRANSLATOR_H
