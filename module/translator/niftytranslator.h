#ifndef NIFTYTRANSLATOR_H
#define NIFTYTRANSLATOR_H

// niftytranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

class NiftyTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(NiftyTranslator)
  typedef NiftyTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit NiftyTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, const QString &to, const QString &from ) override
  { Q_UNUSED(from) Q_UNUSED(to) return createReply(text); }

  QNetworkReply *createReply(const QString &text);

  QString parseReply(const QByteArray &data) override;

  static QString translateUrl(const QString &text);
};

#endif // NIFTYTRANSLATOR_H
