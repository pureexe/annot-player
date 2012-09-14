// googletranslator.cc
// 6/30/2012

#include "lib/translator/googletranslator.h"
#include "lib/translator/googletranslator_p.h"
#include "lib/translator/translatorsettings.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>
//#include <QtWebKit/QWebSettings>

//#define DEBUG "googletranslator"
#include "lib/debug/debug.h"

// - Construction -

QString
GoogleTranslator::name() const
{ return tr("Google Translator"); }

QNetworkAccessManager*
GoogleTranslator::networkAccessManager()
{
  if (!nam_) {
    nam_ = new QNetworkAccessManager(this);
    if (TranslatorSettings::globalSettings()->hasCacheDirectory()) {
      // See: http://www.qtcentre.org/threads/24354-using-QWebView-cache-with-QNetworkDiskCache
      QNetworkDiskCache *cache = new QNetworkDiskCache(nam_);
      cache->setCacheDirectory(TranslatorSettings::globalSettings()->cacheDirectory());
      nam_->setCache(cache);
    }
  }
  return nam_;
}

QWebPage*
GoogleTranslator::createWebPage()
{
  QWebPage *ret = new QWebPage(this);
  //QWebSettings *settings = ret->settings();
  //settings->setAttribute(QWebSettings::AutoLoadImages, false);
  //settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
  //settings->setAttribute(QWebSettings::JavaEnabled, false);
  //settings->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
  //settings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false);
  //settings->setAttribute(QWebSettings::PluginsEnabled, false);
  //settings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, false);
  //settings->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, false);
  //settings->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, false);
  //settings->setAttribute(QWebSettings::LocalStorageEnabled, false);
  //settings->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
  ret->setNetworkAccessManager(networkAccessManager());
  connect(ret, SIGNAL(loadFinished(bool)),
          new detail::ProcessWebPage(ret, this), SLOT(trigger(bool)));
  return ret;
}

//QWebPage*
//GoogleTranslator::allocateWebPage()
//{ return stack_.isEmpty() ? createWebPage() : stack_.pop(); }

//void
//GoogleTranslator::releaseWebPage(QWebPage *page)
//{ stack_.push(page); }

// - Translate -

const char*
GoogleTranslator::lcode(int lang)
{
  switch (lang) {
  case English:  return "en";
  case Japanese: return "ja";
  case TraditionalChinese:  return "zh-TW";
  case SimplifiedChinese:   return "zh-CN";
  case Korean:   return "ko";
  case French:   return "fr";
  case German:   return "de";
  case Italian:  return "it";
  case Spanish:  return "es";
  case Portuguese: return "pt";
  case Russian:  return "ru";
  default: return 0;
  }
}

QString
GoogleTranslator::translateUrl(const QString &text, const char *to, const char *from)
{
  return QString("http://translate.google.com/#%1|%2|%3")
      .arg(from ? from : "auto")
      .arg(to)
      .arg(text);
}

void
GoogleTranslator::doTranslate(const QString &text, int to, int from)
{
  DOUT("enter");
  if (page_ && isSynchronized()) {
    //reply_->abort();
    page_->deleteLater();
    DOUT("abort previous webpage");
  }

  QString url = translateUrl(text, lcode(to), lcode(from));
  DOUT("url =" << url);

  page_ = createWebPage();
  page_->mainFrame()->load(url);
  DOUT("exit");
}

// Sample request: http://api.microsofttranslator.com/v2/Http.svc/Translate?appId=FCB48AFBE3CB7B0E7AA146C950762FC87EA13FBB&text=hello&from=en&to=ja
// Sample reply: <string xmlns="http://schemas.microsoft.com/2003/10/Serialization/">text</string>
void
GoogleTranslator::processWebPage(QWebPage *page, bool success)
{
  DOUT("enter: ok =" << success);

  page->deleteLater();
  if (isSynchronized() && page_ != page) {
    DOUT("exit: reply changed");
    return;
  }
  page_ = nullptr;

  if (success) {
    QWebFrame *f = page->mainFrame();
    QWebElement e = f->findFirstElement("span#result_box");
    if (!e.isNull()) {
      QString t = e.toPlainText();
      if (!t.isEmpty()) {
        emit translated(t);
        DOUT("exit: ok");
        return;
      }
    }
  }
  emit errorMessage(tr("network error from Google Translator"));
  DOUT("exit");
}

// EOF
