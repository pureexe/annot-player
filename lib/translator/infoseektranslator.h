#ifndef INFOSEEKTRANSLATOR_H
#define INFOSEEKTRANSLATOR_H

// infoseektranslator.h
// 8/21/2012

#include "lib/translator/networktranslator.h"

class InfoseekTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(InfoseekTranslator)
  typedef InfoseekTranslator Self;
  typedef NetworkTranslator Base;

  // - Constructions -
public:
  explicit InfoseekTranslator(QObject *parent = nullptr)
    : Base(parent) { }

  QString name() const override;

protected:
  QNetworkReply *createReply(const QString &text, int to, int from) override;
  QString parseReply(const QByteArray &data) override;

protected:
  static QString translateUrl(const QString &text, const char *to, const char *from);
  static const char *lcode(int lang);
};

#endif // INFOSEEKTRANSLATOR_H
