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
  QNetworkReply *createReply(const QString &text, int to, int from) override;

  QString parseReply(const QByteArray &data) override;

protected:
  static QByteArray postData(const QString &text, const char *to, const char *from, const QByteArray &key);
  static QByteArray currentAuthKey();
  static const char *lcode(int lang);
};

#endif // OCNTRANSLATOR_H
