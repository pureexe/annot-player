// main.cc
// 6/30/2011
#include "mainwindow.h"
#include "global.h"
#include "application.h"
#include "rc.h"
#include "settings.h"
#include "project/common/acdownloader.h"
#include "project/common/acsettings.h"
#include "project/common/acpaths.h"
#include "project/common/acui.h"
#include "module/annotcache/annotationcachemanager.h"
#ifdef WITH_MODULE_QT
# include "module/qt/qtrc.h"
#endif // WITH_MODULE_QT
#include "module/mrlresolver/mrlresolversettings.h"
//#include "module/download/downloader.h"
#include <QtGui>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <boost/tuple/tuple.hpp>

#define DEBUG "main"
#include "module/debug/debug.h"

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

#define DEFAULT_SIZE    QSize(400, 300)

// - Startup stages -

namespace { namespace detail {

  // Meta types
  inline void registerMetaTypes()
  {
    // Not registered in Qt 4.8
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
  }

  // Window size

  inline bool isValidWindowSize(const QSize &size, const QWidget *w = nullptr)
  {
    if (size.width() < DEFAULT_SIZE.width() || size.height() < DEFAULT_SIZE.height())
      return false;

    QSize desktop = QApplication::desktop()->availableGeometry(w).size();
    return desktop.isEmpty() ||
           size.width() > desktop.width() && size.height() > desktop.height();
  }

  // i18n

  inline QTranslator *translatorForLanguage(int lang, int script = 0)
  {
    QString qm;
    switch (lang) {
    case QLocale::English: qm = RC_TR_EN; break;
    case QLocale::Japanese: qm = RC_TR_JA; break;
    case QLocale::Chinese: qm = script == QLocale::SimplifiedChineseScript ? RC_TR_ZH_CN : RC_TR_ZH_TW; break;
    default: qm = RC_TR_EN;
    }

    QTranslator *t = new QTranslator;
    if (t->load(qm)) return t;
    else { Q_ASSERT(0); delete t; return 0; }
  }

#ifdef WITH_MODULE_QT
  inline QTranslator *qtTranslatorForLanguage(int lang, int script = 0)
  {
    const char *qm = nullptr;
    switch (lang) {
    case QLocale::Japanese: qm = "qt_ja"; break;
    case QLocale::Chinese: qm = script == QLocale::SimplifiedChineseScript ? "qt_zh_CN" : "qt_zh_TW"; break;
    }

    if (qm) {
      QTranslator *t = new QTranslator(qApp);
      if (t->load(qm, QTRC_PREFIX_TR))
        return t;
      else
        delete t;
    }
    return 0;
  }
#endif // WITH_MODULE_QT

} } // namespace detail

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

  if (!a.isSingleInstance()) {
    DOUT("exit: not single instance");
    AcDownloader delegate;
    QStringList args = a.arguments();
    args.removeFirst()  ;
    if (args.isEmpty())
      delegate.show();
    else
      delegate.openArguments(args);
    a.processEvents();
    return 0;
  }

  // Register meta types.
  detail::registerMetaTypes();

  AcSettings *settings = AcSettings::globalSettings();
  {
    int lang = settings->language(),
        script = settings->languageScript();
    if (!lang ||
        lang == QLocale::Chinese && !script) {
      const QLocale system = QLocale::system();
      lang =  system.language();
      script =  system.script();
      settings->setLanguage(lang, script);
    }
    QTranslator *t = detail::translatorForLanguage(lang, script);
    Q_ASSERT(t);
    if (t)
      a.installTranslator(t);
#ifdef WITH_MODULE_QT
    t = detail::qtTranslatorForLanguage(lang, script);
    if (t)
      a.installTranslator(t);
#endif // WITH_MODULE_QT
  }

  // Check update
  if (Settings::globalSettings()->version() != G_VERSION)
    Settings::globalSettings()->setVersion(G_VERSION);

  // Set theme.
  {
    AcUi *ui = AcUi::globalInstance();
    int tid = settings->themeId();
    ui->setTheme(tid);
    ui->setMenuEnabled(settings->isMenuThemeEnabled());
#ifdef Q_OS_WIN
    ui->setAeroEnabled(settings->isAeroEnabled());
#endif // Q_OS_WIN
  }

  // Set cache location
  AnnotationCacheManager::globalInstance()->setLocation(AC_PATH_CACHES);

  // Set network proxy
  if (settings->isProxyEnabled()) {
    QNetworkProxy::ProxyType type;
    switch (settings->proxyType()) {
    case QNetworkProxy::Socks5Proxy: type = QNetworkProxy::Socks5Proxy; break;
    case QNetworkProxy::HttpProxy: type = QNetworkProxy::HttpProxy; break;
    default: type = QNetworkProxy::Socks5Proxy;
    }
    QNetworkProxy proxy(type, settings->proxyHostName(), settings->proxyPort(),
                        settings->proxyUser(), settings->proxyPassword());
    QNetworkProxy::setApplicationProxy(proxy);
  }

  {
    MrlResolverSettings *ms = MrlResolverSettings::globalSettings();
    QString username, password;

    boost::tie(username, password) = settings->nicovideoAccount();
    ms->setNicovideoAccount(username, password);

    boost::tie(username, password) = settings->bilibiliAccount();
    ms->setBilibiliAccount(username, password);
  }

  //DownloaderController::globalController()->setNetworkAccessManager(
  //  new QNetworkAccessManager(&a)
  //);

  // Allocate more threads
  enum { MinThreadCount = 3 };
  if (QThreadPool::globalInstance()->maxThreadCount() < MinThreadCount)
    QThreadPool::globalInstance()->setMaxThreadCount(MinThreadCount);
  DOUT("thread pool size =" << QThreadPool::globalInstance()->maxThreadCount());

  DOUT("create mainwindow");
  MainWindow w;
  a.setMainWindow(&w);

  QSize sz = Settings::globalSettings()->recentSize();
  if (sz.isEmpty() || !detail::isValidWindowSize(sz, &w))
    sz = DEFAULT_SIZE;
  w.resize(sz);
  w.show();

  QStringList args = a.arguments();
  if (args.size() > 1) {
    args.removeFirst();
    w.promptUrls(args);
  }

  QTimer::singleShot(0, &w, SLOT(checkClipboard()));

  DOUT("exit: exec");
  return a.exec();
}

// EOF
