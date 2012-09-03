#ifndef OCNTRANSLATOR_H
#define OCNTRANSLATOR_H

// ocntranslator.h
// 8/20/2012

#include "module/translator/networktranslator.h"

class OcnTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(OcnTranslator)
  typedef OcnTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit OcnTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, const QString &to, const QString &from) override;

  QString parseReply(const QByteArray &data) override;

protected:
  static QByteArray postData(const QString &text, const QString &to, const QString &from, const QByteArray &key);
  static QByteArray currentAuthKey();
};

#endif // OCNTRANSLATOR_H
