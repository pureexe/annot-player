// main.cc
// 6/30/2011
#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include "global.h"
#include "translatormanager.h"
#include "annotationgraphicsitem.h"
#ifdef WITH_WIN_QTH
#  include "win/qth/qth.h"
#endif // WITH_WIN_QTH
#ifdef WITH_WIN_DWM
#  include "win/dwm/dwm.h"
#endif // WITH_WIN_DWM
#ifdef Q_OS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
#  include "mac/qtmac/qtmac.h"
#endif // Q_OS_MAC
#include "ac/acui.h"
#include "ac/acglobal.h"
#include "ac/acsettings.h"
#include "module/annotcloud/user.h"
#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
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

  // Warm up
  inline void warmUp()
  {
#ifdef Q_OS_WIN
    QtWin::warmUp();
#endif // Q_OS_WIN

#ifdef WITH_WIN_DWM
    Dwm::warmUp();
#endif // WITH_WIN_DWM

    // Cache fonts needed to render annotations
    AnnotationGraphicsItem::warmUp();
  }

} // namespace anonymous

// - Main -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

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
  Settings *settings = Settings::globalSettings();
  AcSettings *ac = AcSettings::globalSettings();

  if (!settings->isMultipleWindowsEnabled() &&
      !a.isSingleInstance())
    return 0;

  // Seed global random generator.
  time_t now = ::time(0);
  ::srand(now);
  ::qsrand(now);

  // Register meta types.
  ::registerMetaTypes();

  // Warm up startup caches
  ::warmUp();

  // Initialize translator
  {
    int lang = ac->language();
    if (!lang) {
      lang =  QLocale::system().language();
      ac->setLanguage(lang);
    }
    TranslatorManager::globalInstance()->setLanguage(lang, false); // auto-update translator = false
    TranslatorManager::globalInstance()->installCurrentTranslator(&a);
    DOUT("app language =" << lang);
  }

  // Rebuild caches on update.
  if (settings->version() != G_VERSION) {
    DOUT("update from old version");
    QFile::remove(G_PATH_CACHEDB);
    QFile::remove(G_PATH_QUEUEDB);
    QFile::remove(G_PATH_DEBUG);
    settings->setVersion(G_VERSION);
  }

  if (ac->version() != AC_VERSION) {
    DOUT("update from old ac version");
    ac->setVersion(AC_VERSION);
  }

  // Hashes
  //qDebug() << qHash(QString("\\bookmark"));
  //qDebug() << qHash(QString("\\b"));
  //qDebug() << qHash(QString("docomo"));
  //qDebug() << qHash(QString("ue"));

  // Load persistant settings

  // Load translations
  //QTranslator t;
  //bool ok = ::setTranslator(t);
  //Q_ASSERT(ok);
  //if (ok && !t.isEmpty())
  //  a.installTranslator(&t);

//#ifdef WITH_MODULE_WEBBROWSER
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

    //ws->setMaximumPagesInCache(10);
    //web->setLocalStoragePath(...);
  }
//#endif // WITH_MODULE_WEBBROWSER

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

//#ifdef USE_MODE_SIGNAL
//  // Root window
//  QMainWindow root; // Persistant visible root widget to prevent Qt from automatic closing invisible windows
//  root.setWindowFlags(root.windowFlags() | Qt::WindowStaysOnTopHint);
//  root.resize(0, 0);
//
//  // Main window
//  MainWindow w(&root, WINDOW_FLAGS);
//  root.show();
//#else
//  MainWindow w;
//
//#endif // USE_MODE_SIGNAL
  DOUT("make mainwindow");
  MainWindow w; {
    Q_ASSERT(w.isValid());
    //if (!w.isValid()) {
    //  DOUT("FATAL ERROR: failed to initialize MainWindow, please contact " G_EMAIL);
    //  return -1;
    //}

//#ifdef WITH_WIN_QTH
//    QTH->setParentWinId(w.winId());
//#endif // WITH_WIN_QTH

    a.setMainWindow(&w);

    // Show main window
    DOUT("show mainwindow");
    w.resize(INIT_WINDOW_SIZE);
    w.show();
    //QTimer::singleShot(0, &w, SLOT(show()));

    // Automatic login
    DOUT("automatic login");
    QString userName = ac->userName(),
            password = ac->password();
    if (userName.isEmpty() || password.isEmpty()) {
      userName = AnnotCloud::User::guest().name();
      password = AnnotCloud::User::guest().password();
    }
    //else
    //  w.checkInternetConnection();
    w.login(userName, password);

    QStringList args = a.arguments();
    if (args.size() > 1)
      w.parseArguments(a.arguments());
  }

  //QWidget t;
  //t.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
  //DWM_ENABLE_AERO_WIDGET((&t));
  //t.showMaximized();

  //w.openPath("d:\\i\\sample.mp4");
  //w.openPath("d:/i/sample.mp4");
  //w.openPath("file:///d:/i/sample.mp4");
  //w.openPath("X:", false);
  //w.openPath("/Volumes/local/i/sample.mp4");
  //w.openPath("dvd://X:", false);
  //w.openPath("cdda://X:", false);
  //w.openPath("/dev/rdisk1", false);

  //Display *dpy = QX11Info::display();
  //XEvent xev;
  //Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
  //Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
//
  //memset(&xev, 0, sizeof(xev));
  //xev.type = ClientMessage;
  //xev.xclient.window = w.winId();
  //xev.xclient.message_type = wm_state;
  //xev.xclient.format = 32;
  //xev.xclient.data.l[0] = 1;
  //xev.xclient.data.l[1] = fullscreen;
  //xev.xclient.data.l[2] = 0;
//
  //XSendEvent(dpy, DefaultRootWindow(dpy), False,
  //SubstructureNotifyMask, &xev);

#if defined(USE_MODE_SIGNAL) && defined(Q_OS_WIN)
  // jichi 11/29/2011: Use as a PERSISTENT hidden top level window.
  QWidget dummy;
  dummy.resize(QSize()); // zero-sized to be hidden
  QObject::connect(&w, SIGNAL(windowClosed()), &dummy, SLOT(close()));

#ifdef WITH_WIN_QTH
  QTH->setParentWinId(dummy.winId());
  QTH->setInterval(200); // Esential!
#endif // WITH_WIN_QTH

  dummy.show();
  //QTimer::singleShot(0, &dummy, SLOT(show()));

#endif // USE_MODE_SIGNAL && Q_OS_WIN

  //QWidget bk;
  //bk.setWindowFlags(Qt::CustomizeWindowHint);
  //DWM_ENABLE_AERO_WIDGET(&bk);
  //bk.showMaximized();

  //QTimer::singleShot(0, &w, SLOT(checkClipboard()));

  DOUT("exit: exec");
  return a.exec();
}

// EOF
