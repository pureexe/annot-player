#ifndef NIFTYTRANSLATOR_H
#define NIFTYTRANSLATOR_H

// niftytranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class NiftyTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(NiftyTranslator)
  typedef NiftyTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit NiftyTranslator(QObject *parent = nullptr)
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

#endif // NIFTYTRANSLATOR_H
