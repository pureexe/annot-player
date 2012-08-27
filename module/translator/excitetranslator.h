#ifndef EXCITETRANSLATOR_H
#define EXCITETRANSLATOR_H

// excitetranslator.h
// 8/20/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class ExciteTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(ExciteTranslator)
  typedef ExciteTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit ExciteTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(0) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override
  { Q_UNUSED(to) Q_UNUSED(from) translate(text); }

  void translate(const QString &text);

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  static QString translateUrl(const QString &text);
};

#endif // EXCITETRANSLATOR_H
