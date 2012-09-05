#ifndef KOTOBANKTRANSLATOR_H
#define KOTOBANKTRANSLATOR_H

// kotobanktranslator.h
// 8/20/2012

#include "module/translator/networktranslator.h"

class KotobankTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(KotobankTranslator)
  typedef KotobankTranslator Self;
  typedef NetworkTranslator Base;

  QString parseReply(const QByteArray &data) override
  { Q_UNUSED(data) return QString(); }

  // - Constructions -
public:
  explicit KotobankTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

  static const char *host() { return "http://kotobank.jp"; }

protected:
  QNetworkReply *createReply(const QString &text, int to, int from) override
  { Q_UNUSED(from) Q_UNUSED(to) return createReply(text); }

  QNetworkReply *createReply(const QString &text);

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  void processSearchData(const QByteArray &data);
  void processWordData(const QByteArray &data);
  static QString translateUrl(const QString &text);
};

#endif // KOTOBANKTRANSLATOR_H
