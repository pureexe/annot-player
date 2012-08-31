#ifndef NETWORKTRANSLATOR_H
#define NETWORKTRANSLATOR_H

// networktranslator.h
// 12/26/2012

#include "module/translator/translator.h"

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class NetworkTranslator : public Translator
{
  Q_OBJECT
  Q_DISABLE_COPY(NetworkTranslator)
  typedef NetworkTranslator Self;
  typedef Translator Base;

  QNetworkAccessManager *nam_;

public:
  explicit NetworkTranslator(QObject *parent = nullptr)
    : Base(parent), nam_(nullptr) { }

protected:
  QNetworkAccessManager *networkAccessManager() const;
protected slots:
  virtual void processReply(QNetworkReply *reply) = 0;
};

#endif // NETWORKTRANSLATOR_H
