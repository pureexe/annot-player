#ifndef FRESHEYETRANSLATOR_H
#define FRESHEYETRANSLATOR_H

// fresheyetranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

class FresheyeTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(FresheyeTranslator)
  typedef FresheyeTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit FresheyeTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, const QString &to, const QString &from) override;

  QString parseReply(const QByteArray &data) override;

  static QByteArray postData(const QString &text, const QString &to, const QString &from);
};

#endif // FRESHEYETRANSLATOR_H
