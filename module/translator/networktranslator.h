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
  QNetworkReply *reply_;

public:
  explicit NetworkTranslator(QObject *parent = nullptr)
    : Base(parent), nam_(nullptr), reply_(nullptr) { }

public slots:
  void translate(const QString &text, int to, int from = 0) override;
protected:
  QNetworkReply *networkReply() const { return reply_; }
  void setNetworkReply(QNetworkReply *reply) { reply_ = reply; }
  QNetworkAccessManager *networkAccessManager() const;
  void doTranslate(const QString &text, int to, int from) override;

protected slots:
  virtual void processReply(QNetworkReply *reply);
protected:
  virtual QNetworkReply *createReply(const QString &text, int to, int from) = 0;
  virtual QString parseReply(const QByteArray &data) = 0;
};

#endif // NETWORKTRANSLATOR_H
