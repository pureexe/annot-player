// sdltranslator.cc
// 8/21/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

#include "module/translator/sdltranslator.h"
#include "module/translator/sdltranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "sdltranslator"
#include "module/debug/debug.h"

// - Properties -

QString
SdlTranslator::name() const
{ return tr("SDL Translator"); }

// - Translate -

QString
SdlTranslator::translateUrl(const QString &text)
{ return SDL_API + text; }

QNetworkReply*
SdlTranslator::createReply(const QString &text)
{ return networkAccessManager()->get(QNetworkRequest(translateUrl(text))); }

QString
SdlTranslator::parseReply(const QByteArray &data)
{ return data; }

// EOF
