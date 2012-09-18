// niftytranslator.cc
// 8/21/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

#include "lib/translator/niftytranslator.h"
#include "lib/translator/niftytranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

//#define DEBUG "niftytranslator"
#include "qtx/qxdebug.h"

// - Properties -

QString
NiftyTranslator::name() const
{ return tr("@nifty Honyaku"); }

// - Translate -

QString
NiftyTranslator::translateUrl(const QString &text)
{ return NIFTY_API + text; }

QNetworkReply*
NiftyTranslator::createReply(const QString &text)
{ return networkAccessManager()->get(QNetworkRequest(translateUrl(text))); }

QString
NiftyTranslator::parseReply(const QByteArray &data)
{
  QByteArray ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(NIFTY_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(NIFTY_AREA_BEGIN) -1;
      int end = data.indexOf(NIFTY_AREA_END, begin);
      if (end > 0)
        ret = data.mid(begin, end - begin);
    }
  }
  return ret;
}

// EOF
