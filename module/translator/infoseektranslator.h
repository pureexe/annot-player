#ifndef INFOSEEKTRANSLATOR_H
#define INFOSEEKTRANSLATOR_H

// infoseektranslator.h
// 8/21/2012

#include "module/translator/networktranslator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class InfoseekTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(InfoseekTranslator)
  typedef InfoseekTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit InfoseekTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(0) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected slots:
  void processReply(QNetworkReply *reply) override;

protected:
  static QString translateUrl(const QString &text, const char *to, const char *from);
  static const char *translateLanguage(const QString &lcode);
};

#endif // INFOSEEKTRANSLATOR_H
