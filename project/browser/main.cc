// main.cc
// 6/30/2011
#include "mainwindow.h"
#include "application.h"
#include "rc.h"
#include "settings.h"
#include "global.h"
#include "project/common/acsettings.h"
#include "project/common/acui.h"
#ifdef WITH_MODULE_QT
#  include "module/qt/qtrc.h"
#endif // WITH_MODULE_QT
#include <QtWebKit/QWebSettings>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QtCore>
#include <ctime>
#include <cstdlib>

#define DEBUG "main"
#include "module/debug/debug.h"

// - Startup stages -

namespace { // anonymous

  // Meta types
  inline void registerMetaTypes()
  { // Not registered in Qt 4.8
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
  }

  // i18n

  inline QTranslator *translatorForLanguage(int lang)
  {
    QString qm;
    switch (lang) {
    case QLocale::English: qm = RC_TR_EN; break;
    case QLocale::Japanese: qm = RC_TR_JA; break;
    case QLocale::Chinese: qm = RC_TR_ZH; break;
    case QLocale::Taiwan: qm = RC_TR_TW; break;
    default: qm = RC_TR_EN;
    }

    QTranslator *t = new QTranslator(qApp);
    if (t->load(qm)) return t;
    else { Q_ASSERT(0); delete t; return 0; }
  }

#ifdef WITH_MODULE_QT
  inline QTranslator *qtTranslatorForLanguage(int lang)
  {
    QString qm;
    switch (lang) {
    case QLocale::English: break;
    case QLocale::Japanese: qm = "qt_ja"; break;
    case QLocale::Chinese: qm = "qt_zh_CN"; break;
    case QLocale::Taiwan: qm = "qt_zh_TW"; break;
    }

    if (!qm.isEmpty()) {
      QTranslator *t = new QTranslator(qApp);
      if (t->load(qm, QTRC_PREFIX_TR))
        return t;
      else
        delete t;
    }
    return 0;
  }
#endif // WITH_MODULE_QT

} // namespace anonymous

// - Main -

int
main(int argc, char *argv[])
{
  DOUT("enter");
  // Set OS encoding to UTF-8 before application starts.
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForCStrings(codec);
  QTextCodec::setCodecForTr(codec);

  // Applications
  Application a(argc, argv);
  // Seed global random generator.
  time_t now = ::time(0);
  ::srand(now);
  ::qsrand(now);

  // Register meta types.
  ::registerMetaTypes();

  AcSettings *ac = AcSettings::globalSettings();
  Settings *settings = Settings::globalSettings();
  {
    int lang = ac->language();
    if (!lang) {
      lang =  QLocale::system().language();
      ac->setLanguage(lang);
    }
    QTranslator *t = translatorForLanguage(lang);
    Q_ASSERT(t);
    if (t)
      a.installTranslator(t);
#ifdef WITH_MODULE_QT
    t = qtTranslatorForLanguage(lang);
    if (t)
      a.installTranslator(t);
#endif // WITH_MODULE_QT
  }

  // Check update
  if (settings->version() != G_VERSION) {
    settings->setFullScreen(false);
    settings->setVersion(G_VERSION);
    settings->sync();
  }

  // Set theme.
  {
    AcUi *ui = AcUi::globalInstance();
    int tid = ac->themeId();
    ui->setTheme(tid);
    ui->setMenuEnabled(ac->isMenuThemeEnabled());
#ifdef Q_WS_WIN
    ui->setAeroEnabled(ac->isAeroEnabled());
#endif // Q_WS_WIN
  }

  // Set webkit settings
  {
    QWebSettings *ws = QWebSettings::globalSettings();
    ws->setAttribute(QWebSettings::PluginsEnabled, true);
    ws->setAttribute(QWebSettings::JavaEnabled, true);
    ws->setAttribute(QWebSettings::DnsPrefetchEnabled, true); // better performance

    ws->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    ws->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    ws->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    ws->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    ws->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);

    ws->setAttribute(QWebSettings::LocalStorageEnabled, true);
    ws->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);

    ws->setAttribute(QWebSettings::ZoomTextOnly, false);

    ws->setDefaultTextEncoding("SHIFT-JIS");
    //ws->setDefaultTextEncoding("EUC-JP");

    ws->setLocalStoragePath(G_PATH_CACHES);
    ws->setIconDatabasePath(G_PATH_CACHES);
    ws->setOfflineStoragePath(G_PATH_CACHES);
    ws->setOfflineWebApplicationCachePath(G_PATH_CACHES);

    // See: http://webkit.org/blog/427/webkit-page-cache-i-the-basics/
    ws->setMaximumPagesInCache(30);
  }

  //void *p = dlopen("/Library/Internet Plug-Ins/QuickTime Plugin.plugin/Contents/MacOS/QuickTime Plugin", 265);

  // Set network proxy
  if (ac->isProxyEnabled()) {
    QNetworkProxy::ProxyType type;
    switch (ac->proxyType()) {
    case QNetworkProxy::Socks5Proxy: type = QNetworkProxy::Socks5Proxy; break;
    case QNetworkProxy::HttpProxy: type = QNetworkProxy::HttpProxy; break;
    default: type = QNetworkProxy::Socks5Proxy;
    }
    QNetworkProxy proxy(type, ac->proxyHostName(), ac->proxyPort(),
                        ac->proxyUser(), ac->proxyPassword());
    QNetworkProxy::setApplicationProxy(proxy);
  }

  DOUT("make mainwindow");

  QStringList args = a.arguments();
  if (args.size() > 1) {
    args.removeFirst();
    QStringList urls = settings->recentTabs();
    urls.append(args);
    settings->setRecentTabs(args);
    settings->setRecentTabIndex(urls.size() - 1);
  }

  MainWindow *w = new MainWindow;

  //QTimer::singleShot(0, w, SLOT(login()));
  w->login();
  w->show();

  DOUT("exit: exec");
  return a.exec();
}

// EOF
