// main.cc
// 8/13/2011
#include "mainwindow.h"
#include "global.h"
#include "application.h"
#include "settings.h"
#include "src/common/actranslator.h"
#include "src/common/acsettings.h"
#include "src/common/acpaths.h"
#include "src/common/acui.h"
#include "lib/translator/translatorsettings.h"
#include "lib/translator/translatormanager.h"
#ifdef WITH_LIB_MECAB
# include "lib/mecab/mecabsettings.h"
#endif // WITH_LIB_MECAB
#include <QtGui>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>

#define DEBUG "main"
#include "qtx/qxdebug.h"

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

#define DEFAULT_SIZE    QSize(300, 200) // minimal size

enum { DEFAULT_TRANSLATORS = TranslatorManager::RomajiBit | TranslatorManager::InfoseekBit };

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
//#ifdef Q_OS_WIN
  QDir::setCurrent(QCoreApplication::applicationDirPath());
//#endif // Q_OS_WIN
  a.loadTranslations();

  if (!a.isSingleInstance()) {
    DOUT("exit: not single instance");
    AcTranslator delegate;
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

  // Check update
  if (Settings::globalSettings()->version() != G_VERSION) {
    Settings::globalSettings()->setTranslationServices(DEFAULT_TRANSLATORS);
    Settings::globalSettings()->setVersion(G_VERSION);
    Settings::globalSettings()->sync();
  }

  AcSettings *settings = AcSettings::globalSettings();

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
  TranslatorSettings::globalSettings()->setCacheDirectory(G_PATH_CACHES);
  TranslatorSettings::globalSettings()->setCacheSize(20);

#ifdef WITH_LIB_MECAB
  // MeCab
  MeCabSettings::setMeCabRcFile(G_PATH_MECABRC);
  //DOUT("mecabrc =" << MeCabSettings::getMeCabRcFile());
#endif // WITH_LIB_MECAB

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
    w.showTranslate(args);
  }

  QTimer::singleShot(0, &w, SLOT(translateClipboard()));

  DOUT("exit: exec");
  return a.exec();
}

// EOF
