// excitetranslator.cc
// 8/20/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

#include "lib/translator/excitetranslator.h"
#include "lib/translator/excitetranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

//#define DEBUG "excitetranslator"
#include "qtx/qxdebug.h"

// - Properties -

QString
ExciteTranslator::name() const
{ return tr("Excite Honyaku"); }

// - Translate -

QString
ExciteTranslator::translateUrl(const QString &text)
{ return EXCITE_API + text; }

QNetworkReply*
ExciteTranslator::createReply(const QString &text)
{ return networkAccessManager()->get(QNetworkRequest(translateUrl(text))); }

QString
ExciteTranslator::parseReply(const QByteArray &data)
{
  QByteArray ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(EXCITE_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(EXCITE_AREA_BEGIN) -1;
      begin = data.indexOf(EXCITE_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(EXCITE_AREA_BEGIN2) -1;
        int end = data.indexOf(EXCITE_AREA_END, begin);
        if (end > 0) {
          ret = data.mid(begin, end - begin);
          ret.replace("&#010;", "\n");
          if (ret[ret.size() - 1] == '\n')
            ret.chop(1);
        }
      }
    }
  }
  return ret;
}

// EOF
