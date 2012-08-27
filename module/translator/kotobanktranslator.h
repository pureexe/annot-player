#ifndef KOTOBANKTRANSLATOR_H
#define KOTOBANKTRANSLATOR_H

// kotobanktranslator.h
// 8/20/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class KotobankTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(KotobankTranslator)
  typedef KotobankTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit KotobankTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(0) { }

  static const char *host() { return "http://kotobank.jp"; }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override
  { Q_UNUSED(from) Q_UNUSED(to) translate(text); }

  void translate(const QString &text);

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  void processSearchData(const QByteArray &data);
  void processWordData(const QByteArray &data);
  static QString translateUrl(const QString &text);
};

#endif // KOTOBANKTRANSLATOR_H
