// googletranslator.cc
// 6/30/2012

#include "module/translator/googletranslator.h"
#include "module/translator/googletranslator_p.h"
#include "module/translator/translatorsettings.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>
//#include <QtWebKit/QWebSettings>

//#define DEBUG "googletranslator"
#include "module/debug/debug.h"

// - Construction -

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

QString
GoogleTranslator::translateUrl(const QString &text, const QString &to, const QString &from)
{
  return QString("http://translate.google.com/#%1|%2|%3")
      .arg(from.isEmpty() ? QString("auto") : from)
      .arg(to == "zh-CHS" ? QString("zh-CN") : to == "zh-CHT" ? QString("zh-TW") : to)
      .arg(text);
}

void
GoogleTranslator::translate(const QString &text, const QString &to, const QString &from)
{
  if (!isEnabled())
    return;
  DOUT("enter");

  if (page_ && isSynchronized()) {
    //reply_->abort();
    page_->deleteLater();
    DOUT("abort previous webpage");
  }

  QString url = translateUrl(text, to, from);
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
