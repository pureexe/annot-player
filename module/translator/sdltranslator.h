#ifndef SDLTRANSLATOR_H
#define SDLTRANSLATOR_H

// sdltranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class SdlTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(SdlTranslator)
  typedef SdlTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit SdlTranslator(QObject *parent = nullptr)
   : Base(parent), reply_(0) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override
  { Q_UNUSED(from) Q_UNUSED(to) translate(text); }

  void translate(const QString &text);

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  static QString translateUrl(const QString &text);
};

#endif // SDLTRANSLATOR_H
