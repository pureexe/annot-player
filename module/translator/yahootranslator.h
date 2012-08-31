#ifndef YAHOOTRANSLATOR_H
#define YAHOOTRANSLATOR_H

// yahootranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class YahooTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(YahooTranslator)
  typedef YahooTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit YahooTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(nullptr) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  static QString translateUrl(const QString &text, const char *to, const char *from);
  static const char *translateLanguage(const QString &lcode);
};

#endif // YAHOOTRANSLATOR_H
