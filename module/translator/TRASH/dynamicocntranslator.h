#ifndef DYNAMICOCNTRANSLATOR_H
#define DYNAMICOCNTRANSLATOR_H

// dynamicocntranslator.h
// 8/20/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class DynamicOcnTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(DynamicOcnTranslator)
  typedef DynamicOcnTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit DynamicOcnTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(nullptr) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected slots:
  void processReply(QNetworkReply *reply) override;
  void processData(const QByteArray &data);

protected:
  static QByteArray postData(const QString &text, const QString &to, const QString &from, const QByteArray &key);
  static QByteArray parseAuthKey(const QByteArray &data);
};

#endif // DYNAMICOCNTRANSLATOR_H
