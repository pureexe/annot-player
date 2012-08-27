#ifndef FRESHEYETRANSLATOR_H
#define FRESHEYETRANSLATOR_H

// fresheyetranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class FresheyeTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(FresheyeTranslator)
  typedef FresheyeTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit FresheyeTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(0) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  static QByteArray postData(const QString &text, const QString &to, const QString &from);
};

#endif // FRESHEYETRANSLATOR_H
