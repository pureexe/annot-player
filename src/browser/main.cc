// main.cc
// 6/30/2011
#include "mainwindow.h"
#include "application.h"
#include "settings.h"
#include "global.h"
#include "src/common/acpaths.h"
#include "src/common/acsettings.h"
#include "src/common/acui.h"
//#include "lib/download/downloader.h"
#include "lib/annotcache/annotationcachemanager.h"
#include "lib/mstypes/lcid.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include <QtGui>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
//#include <QtNetwork/QNetworkAccessManager>
#include <QtWebKit/QWebSettings>

#define DEBUG "main"
#include "qtx/qxdebug.h"

#ifdef __clang__
# pragma clang diagnostic ignored "-Wlogical-op-parentheses" // '&&' within '||'
#endif // __clang__

// - Startup stages -

namespace { namespace detail {

  // Meta types
  inline void registerMetaTypes()
  { // Not registered in Qt 4.8
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
  }

} } // anonymous detail

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
#ifdef Q_OS_WIN
  QDir::setCurrent(QCoreApplication::applicationDirPath());
#endif // Q_OS_WIN
  a.loadTranslations();

  // Register meta types.
  detail::registerMetaTypes();

  AcSettings *ac = AcSettings::globalSettings();
  Settings *settings = Settings::globalSettings();

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
#ifdef Q_OS_WIN
    ui->setAeroEnabled(ac->isAeroEnabled());
#endif // Q_OS_WIN
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

  // Set annotation cache directory, shared across different annot apps
  AnnotationCacheManager::globalInstance()->setLocation(AC_PATH_CACHES);

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

  //DownloaderController::globalController()->setNetworkAccessManager(
  //  new QNetworkAccessManager(&a)
  //);

  QStringList args = a.arguments();
  if (args.size() > 1) {
    args.removeFirst();
    QStringList urls = settings->recentTabs();
    urls.append(args);
    settings->setRecentTabs(args);
    settings->setRecentTabIndex(urls.size() - 1);
  }

  // Allocate more threads
  enum { MinThreadCount = 3 };
  if (QThreadPool::globalInstance()->maxThreadCount() < MinThreadCount)
    QThreadPool::globalInstance()->setMaxThreadCount(MinThreadCount);
  DOUT("thread pool size =" << QThreadPool::globalInstance()->maxThreadCount());

  DOUT("create mainwindow");
  MainWindow *w = new MainWindow;

  //QTimer::singleShot(0, w, SLOT(login()));
  //w->login();

  DOUT("showing mainwindow");
  w->show();

  DOUT("exit: exec");
  return a.exec();
}

// EOF
