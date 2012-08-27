#ifndef MICROSOFTTRANSLATOR_H
#define MICROSOFTTRANSLATOR_H

// microsofttranslator.h
// 11/2/2011

#include "module/translator/networktranslator.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>

QT_FORWARD_DECLARE_CLASS(QAuthenticator)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class MicrosoftTranslator : public NetworkTranslator
{
  Q_OBJECT
  Q_DISABLE_COPY(MicrosoftTranslator)
  typedef MicrosoftTranslator Self;
  typedef NetworkTranslator Base;

  QNetworkReply *reply_;

  // - Constructions -
public:
  explicit MicrosoftTranslator(QObject *parent = nullptr);

public slots:
  void translate(const QString &text, const QString &to, const QString &from = QString()) override;

protected slots:
  void processReply(QNetworkReply *reply) override;
  void authenticate(QNetworkReply *, QAuthenticator *auth) const { authenticate(auth); }

protected:
  static void authenticate(QAuthenticator *auth);
  static QUrl translateUrl(const QString &text, const QString &to, const QString &from = QString());
  static QNetworkRequest translateRequest(const QUrl &url);
};

#endif // MICROSOFTTRANSLATOR_H
