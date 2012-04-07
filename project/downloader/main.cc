// main.cc
// 6/30/2011
#include "mainwindow.h"
#include "application.h"
#include "rc.h"
#include "ac/acsettings.h"
#include "ac/acui.h"
#ifdef WITH_MODULE_QT
#  include "module/qt/qtrc.h"
#endif // WITH_MODULE_QT
#include "module/mrlresolver/mrlresolversettings.h"
#include <QtGui>
#include <QtNetwork>
#include <boost/tuple/tuple.hpp>
#include <ctime>
#include <cstdlib>

#define DEBUG "main"
#include "module/debug/debug.h"

// - Startup stages -

namespace { // anonymous

  // Meta types
  inline void registerMetaTypes()
  {
    // Not registered in Qt 4.8
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

    QTranslator *t = new QTranslator;
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

  if (!a.isSingleInstance()) {
    DOUT("exit: not single instance");
    return 0;
  }

  // Seed global random generator.
  time_t now = ::time(0);
  ::srand(now);
  ::qsrand(now);

  // Register meta types.
  ::registerMetaTypes();

  AcSettings *settings = AcSettings::globalSettings();
  {
    int lang = settings->language();
    if (!lang) {
      lang =  QLocale::system().language();
      settings->setLanguage(lang);
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

  // Set theme.
  {
    AcUi *ui = AcUi::globalInstance();
    int tid = settings->themeId();
    ui->setTheme(tid);
    ui->setMenuEnabled(settings->isMenuThemeEnabled());
#ifdef Q_WS_WIN
    ui->setAeroEnabled(settings->isAeroEnabled());
#endif // Q_WS_WIN
  }

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

  DOUT("make mainwindow");
  MainWindow w;
  a.setMainWindow(&w);

  w.resize(400, 300);
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
