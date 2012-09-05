// systrantranslator.cc
// 9/4/2012
// See: ciitran/translation.cs

#include "module/translator/systrantranslator.h"
#include "module/translator/systrantranslator_p.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "systrantranslator"
#include "module/debug/debug.h"

QString
SystranTranslator::name() const
{ return tr("SYSTRAN Translator"); }

QNetworkReply*
SystranTranslator::createReply(const QString &text)
{
  QNetworkRequest req(QString(SYSTRAN_API));
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  QString t = QString(text).replace('\\n', ' ');
  return networkAccessManager()->post(req, t.toUtf8());
}

QString
SystranTranslator::parseReply(const QByteArray &data)
{
  DOUT("enter: data =" << data);
  QByteArray ret;
  if (!data.isEmpty() && data.startsWith(SYSTRAN_REPLY_BEGIN))
    ret = data.mid(sizeof(SYSTRAN_REPLY_BEGIN) -1);
  DOUT("exit");
  return ret;
}

// EOF
