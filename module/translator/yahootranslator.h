#ifndef YAHOOTRANSLATOR_H
#define YAHOOTRANSLATOR_H

// yahootranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

class YahooTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(YahooTranslator)
  typedef YahooTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit YahooTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, const QString &to, const QString &from) override;

  QString parseReply(const QByteArray &data) override;

protected:
  static QString translateUrl(const QString &text, const char *to, const char *from);
  static const char *translateLanguage(const QString &lcode);
};

#endif // YAHOOTRANSLATOR_H
