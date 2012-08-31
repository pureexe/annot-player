#ifndef OCNTRANSLATOR_H
#define OCNTRANSLATOR_H

// ocntranslator.h
// 8/20/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class OcnTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(OcnTranslator)
  typedef OcnTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit OcnTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(nullptr) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  static QByteArray postData(const QString &text, const QString &to, const QString &from, const QByteArray &key);
  static QByteArray currentAuthKey();
};

#endif // OCNTRANSLATOR_H
