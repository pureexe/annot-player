#ifndef MICROSOFTTRANSLATOR_H
#define MICROSOFTTRANSLATOR_H

// microsofttranslator.h
// 11/2/2011

#include "module/translator/translator.h"
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>

QT_FORWARD_DECLARE_CLASS(QAuthenticator)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class MicrosoftTranslator : public Translator
{
  Q_OBJECT
  Q_DISABLE_COPY(MicrosoftTranslator)
  typedef MicrosoftTranslator Self;
  typedef Translator Base;

  QNetworkAccessManager *qnam_;

  // - Constructions -
public:
  explicit MicrosoftTranslator(QObject *parent = 0);

public slots:
  virtual void translate(const QString &text, const QString &to, const QString &from = QString()); ///< \reimp

protected slots:
  void processNetworkReply(QNetworkReply *reply);
  void authenticate(QNetworkReply *, QAuthenticator *auth) const { authenticate(auth); }

protected:
  static void authenticate(QAuthenticator *auth);
  static QUrl translateUrl(const QString &text, const QString &to, const QString &from = QString());
  static QNetworkRequest translateRequest(const QUrl &url);
};

#endif // MICROSOFTTRANSLATOR_H
