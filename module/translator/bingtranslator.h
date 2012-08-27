#ifndef BINGTRANSLATOR_H
#define BINGTRANSLATOR_H

// bingtranslator.h
// 11/2/2011

#include "module/translator/networktranslator.h"
#include <QtCore/QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class BingTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(BingTranslator)
  typedef BingTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit BingTranslator(QObject *parent = nullptr)
    : Base(parent), reply_(0) { }

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected:
  static QUrl translateUrl(const QString &text, const QString &to, const QString &from = QString());
protected slots:
  void processReply(QNetworkReply *reply) override;
};

#endif // BINGTRANSLATOR_H
