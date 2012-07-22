#ifndef BINGTRANSLATOR_H
#define BINGTRANSLATOR_H

// bingtranslator.h
// 11/2/2011

#include "module/translator/translator.h"
#include <QtCore/QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class BingTranslator : public Translator
{
  Q_OBJECT
  Q_DISABLE_COPY(BingTranslator)
  typedef BingTranslator Self;
  typedef Translator Base;

  QNetworkAccessManager *qnam_;

  // - Constructions -
public:
  explicit BingTranslator(QObject *parent = nullptr);

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected:
  static QUrl translateUrl(const QString &text, const QString &to, const QString &from = QString());
protected slots:
  void processNetworkReply(QNetworkReply *reply);
};

#endif // BINGTRANSLATOR_H
