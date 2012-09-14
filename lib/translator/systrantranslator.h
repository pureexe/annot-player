#ifndef SYSTRANTRANSLATOR_H
#define SYSTRANTRANSLATOR_H

// systrantranslator.h
// 9/4/2012

#include "lib/translator/networktranslator.h"

class SystranTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(SystranTranslator)
  typedef SystranTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit SystranTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, int to, int from) override
  { Q_UNUSED(to) Q_UNUSED(from) return createReply(text); }

  QNetworkReply *createReply(const QString &text);

  QString parseReply(const QByteArray &data) override;
};

#endif // SYSTRANTRANSLATOR_H
