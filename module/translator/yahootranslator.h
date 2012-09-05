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
  QNetworkReply *createReply(const QString &text, int to, int from) override;

  QString parseReply(const QByteArray &data) override;

protected:
  static QString translateUrl(const QString &text, const char *to, const char *from);
  static const char *lcode(int lang);
};

#endif // YAHOOTRANSLATOR_H
