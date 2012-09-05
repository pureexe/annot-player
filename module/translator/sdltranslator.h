#ifndef SDLTRANSLATOR_H
#define SDLTRANSLATOR_H

// sdltranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

class SdlTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(SdlTranslator)
  typedef SdlTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit SdlTranslator(QObject *parent = nullptr)
   : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, int to, int from) override
  { Q_UNUSED(from) Q_UNUSED(to) return createReply(text); }

  QNetworkReply *createReply(const QString &text);

  QString parseReply(const QByteArray &data) override;

protected:
  static QString translateUrl(const QString &text);
};

#endif // SDLTRANSLATOR_H
