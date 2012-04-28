// main.cc
// 6/30/2011
#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include "global.h"
#include "tr.h"
#include "translatormanager.h"
#include "annotationgraphicsitem.h"
#ifdef Q_WS_WIN
#  include "windowsregistry.h"
#  include "module/player/player.h"
#  include "win/qtwin/winreg.h"
#endif // Q_WS_WIN
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
#include "project/common/acui.h"
#include "project/common/acglobal.h"
#include "project/common/acsettings.h"
#include "project/common/acplayer.h"
#include "module/annotcloud/user.h"
#include <QtWebKit/QWebSettings>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QDesktopServices>
#include <QtCore>
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

  // Register file types
  inline void registerFileTypes()
  {
#ifdef Q_WS_WIN
    WindowsRegistry reg;
    reg.setClassesRoot(REG_HKCU_SOFTWARE_CLASSES);
    reg.registerFileTypes(Player::supportedAudioSuffices());
    reg.registerFileTypes(Player::supportedVideoSuffices());
    reg.registerFileTypes(Player::supportedPictureSuffices());
    reg.registerFileTypes(Player::supportedSubtitleSuffices());
#endif // Q_WS_WIN
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

  bool unique = a.isSingleInstance();
  if (!unique && !settings->isMultipleWindowsEnabled()) {
    QStringList args = a.arguments();
    args.removeFirst();
    if (!args.isEmpty()) {
      AcPlayer delegate;
      delegate.openArguments(args);
      a.processEvents();
    }
    return 0;
  }

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
      if (lang == QLocale::Japanese) {
        settings->setAnnotationLanguages( // Ban Chinese language
          Traits::JapaneseBit | Traits::UnknownLanguageBit |
          Traits::EnglishBit | Traits::KoreanBit
        );
        settings->setAnnotationFilterEnabled(true);
      }
      ac->setLanguage(lang);
    }
    TranslatorManager::globalInstance()->setLanguage(lang, false); // auto-update translator = false
    TranslatorManager::globalInstance()->installCurrentTranslator(&a);
    DOUT("app language =" << lang);
  }

  // Check update.
  if (settings->version() != G_VERSION) {
    DOUT("update from old version");
    QFile::remove(G_PATH_CACHEDB);
    QFile::remove(G_PATH_QUEUEDB);
    QFile::remove(G_PATH_DEBUG);
    settings->setVersion(G_VERSION);

    settings->setWindowOnTop(false);
    settings->setApplicationFilePath(QString());
    //settings->setAnnotationLanguages(Traits::AllLanguages);
    //settings->setAnnotationFilterEnabled(false);
    settings->sync();

    if (QFile::rename(QDir::homePath() + "/Annot", G_PATH_DOWNLOADS))
      AcLocationManager::globalInstance()->createDownloadsLocation();
  }

  // Moved
  if (settings->applicationFilePath() != QCoreApplication::applicationFilePath()) {
    registerFileTypes();
    settings->setApplicationFilePath(QCoreApplication::applicationFilePath());
    settings->sync();
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

    ws->setLocalStoragePath(G_PATH_CACHES);
    ws->setIconDatabasePath(G_PATH_CACHES);
    ws->setOfflineStoragePath(G_PATH_CACHES);
    ws->setOfflineWebApplicationCachePath(G_PATH_CACHES);

    // See: http://webkit.org/blog/427/webkit-page-cache-i-the-basics/
    ws->setMaximumPagesInCache(10);
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
//  root.resize(QSize());
//
//  // Main window
//  MainWindow w(&root, WINDOW_FLAGS);
//  root.show();
//#else
//  MainWindow w;
//
//#endif // USE_MODE_SIGNAL
  DOUT("make mainwindow");
  MainWindow w(unique); {
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
    if (args.size() > 1) {
      args.removeFirst();
      w.openSources(args);
    }
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

  //QTimer::singleShota(0, &w, SLOT(checkClipboard()));

  DOUT("exit: exec");
  return a.exec();
}

// EOF
