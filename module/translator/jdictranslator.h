#ifndef JDICTRANSLATOR_H
#define JDICTRANSLATOR_H

// jdictranslator.h
// 8/11/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QTextEncoder)
QT_FORWARD_DECLARE_CLASS(QTextDecoder)

class JdicTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(JdicTranslator)
  typedef JdicTranslator Self;
  typedef NetworkTranslator Base;

  QTextDecoder *decoder_;
  QTextEncoder *encoder_;

  // - Constructions -
public:
  explicit JdicTranslator(QObject *parent = nullptr);

  QString name() const override;

  static const char *dictionaryForLanguage(const QString &lang);

protected:
  QNetworkReply *createReply(const QString &text, const QString &to, const QString &from) override
  { Q_UNUSED(from) return createReply(text, dictionaryForLanguage(to)); }

  QNetworkReply *createReply(const QString &text, const char *dict = 0);

  QString parseReply(const QByteArray &data) override;

protected:
  QByteArray postData(const QString &text, const char *dict) const;

  QString decodeText(const QByteArray &data) const;
  QByteArray encodeText(const QString &text) const;
};

#endif // JDICTRANSLATOR_H
