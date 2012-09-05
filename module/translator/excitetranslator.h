#ifndef EXCITETRANSLATOR_H
#define EXCITETRANSLATOR_H

// excitetranslator.h
// 8/20/2012

#include "module/translator/networktranslator.h"

class ExciteTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(ExciteTranslator)
  typedef ExciteTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit ExciteTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, int to, int from) override
  { Q_UNUSED(to) Q_UNUSED(from) return createReply(text); }

  QNetworkReply *createReply(const QString &text);

  QString parseReply(const QByteArray &data) override;

  static QString translateUrl(const QString &text);
};

#endif // EXCITETRANSLATOR_H
