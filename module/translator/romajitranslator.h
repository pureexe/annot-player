#ifndef ROMAJITRANSLATOR_H
#define ROMAJITRANSLATOR_H

// romajitranslator.h
// 7/15/2012

#include "module/translator/translator.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class QTextEncoder;
class QTextDecoder;
QT_END_NAMESPACE

class RomajiTranslator : public Translator
{
  Q_OBJECT
  Q_DISABLE_COPY(RomajiTranslator)
  typedef RomajiTranslator Self;
  typedef Translator Base;

  QNetworkAccessManager *qnam_;
  QTextDecoder *decoder_;
  QTextEncoder *encoder_;

  // - Constructions -
public:
  explicit RomajiTranslator(QObject *parent = nullptr);

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override
  { translate(text); Q_UNUSED(to); Q_UNUSED(from); }

  void translate(const QString &text);

protected slots:
  void processNetworkReply(QNetworkReply *reply);

protected:
  QString translateUrl(const QString &text) const;

  QString decodeText(const QByteArray &data) const;
  QByteArray encodeText(const QString &text) const;
};

#endif // ROMAJITRANSLATOR_H
