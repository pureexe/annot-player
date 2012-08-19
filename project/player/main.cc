// main.cc
// 6/30/2011
#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include "annotationsettings.h"
#include "global.h"
#include "tr.h"
#include "translationmanager.h"
#include "annotationgraphicsitem.h"
#include "module/translator/translatorsettings.h"
#ifdef Q_OS_WIN
# include "windowsregistry.h"
# include "module/player/player.h"
#endif // Q_OS_WIN
#ifdef WITH_WIN_TEXTHOOK
# include "win/texthook/texthook.h"
#endif // WITH_WIN_TEXTHOOK
#ifdef WITH_WIN_DWM
# include "win/dwm/dwm.h"
#endif // WITH_WIN_DWM
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
# include "mac/qtmac/qtmac.h"
#endif // Q_OS_MAC
#include "project/common/acui.h"
#include "project/common/acglobal.h"
#include "project/common/acsettings.h"
#include "project/common/acplayer.h"
#include "module/qtext/filesystem.h"
#include "module/annotcloud/user.h"
#include "module/annotcache/annotationcachemanager.h"
//#include "module/qt/qtsettings.h"
//#include "module/download/downloader.h"
#include <QtGui>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QtWebKit/QWebSettings>
//#include <fontconfig/fontconfig.h>
//#include <freetype/ftsynth.h>
//#include <freetype/freetype.h>
//#include <freetype/ftglyph.h>
//#include <freetype/ftstroke.h>

//#include <vlc/plugins/vlc_common.h>
//extern "C" {
//# include <vlc/src/modules/modules.h>
//module_t *module_find(const char *name);
//} // extern "C"

#define DEBUG "main"
#include "module/debug/debug.h"

#ifdef __clang__
# pragma clang diagnostic ignored "-Wlogical-op-parentheses" // '&&' within '||'
#endif // __clang__

// - Startup stages -

namespace { namespace detail {

  // Meta types
  inline void registerMetaTypes()
  {
    // Not registered in Qt 4.8
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
  }

  // Register file types
  inline void registerAssociations()
  {
#ifdef Q_OS_WIN
    WindowsRegistry *reg = WindowsRegistry::globalInstance();
    //reg->registerTypes(Player::supportedAudioSuffices());
    reg->registerTypes(Player::supportedVideoSuffices());
    //reg.registerTypes(Player::supportedPictureSuffices());
    reg->registerTypes(Player::supportedImageSuffices());
    reg->registerTypes(Player::supportedSubtitleSuffices());
    reg->registerTypes(Player::supportedPlaylistSuffices());

    //reg->registerTypes(QStringList() G_FORMAT_PROGRAM(<<));
    //reg->registerTypes(QStringList() G_FORMAT_ANNOTATION(<<));

    //reg->registerRawTypes(QStringList() G_FORMAT_DEVICE(<<));
    reg->registerShell("DVD");
#endif // Q_OS_WIN
  }

  inline void repairAssociations()
  {
#ifdef Q_OS_WIN
    WindowsRegistry *reg = WindowsRegistry::globalInstance();
    foreach (const QString &type, Player::supportedSuffices())
      if (reg->containsType(type))
        reg->registerType(type);

    foreach (const QString &type, QStringList() G_FORMAT_PROGRAM(<<) G_FORMAT_ANNOTATION(<<))
      if (reg->containsType(type))
        reg->registerType(type);

    foreach (const QString &type, QStringList() G_FORMAT_DEVICE(<<))
      if (reg->containsShell(type))
        reg->registerShell(type);
#endif // Q_OS_WIN
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
    //DOUT("default annotation font =" << AnnotationSettings::globalSettings()->fontFamily());
    //DOUT("current annotation font =" << font);
    AnnotationSettings::globalSettings()->setFontFamily(Settings::globalSettings()->annotationFontFamily());
    AnnotationSettings::globalSettings()->setJapaneseFontFamily(Settings::globalSettings()->annotationJapaneseFontFamily());
    AnnotationSettings::globalSettings()->setChineseFontFamily(Settings::globalSettings()->annotationChineseFontFamily());
    AnnotationGraphicsItem::warmUp();
  }

} } // namespace detail

// - Main -

//#define WINDOW_FLAGS (
//  Qt::Dialog |
//  Qt::CustomizeWindowHint
//  Qt::WindowTitleHint |
//  Qt::WindowSystemMenuHint |
//  Qt::WindowMinMaxButtonsHint |
//  Qt::WindowCloseButtonHint )

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

// Delayed after args are parsed
//#ifdef Q_OS_WIN
//  QDir::setCurrent(QCoreApplication::applicationDirPath());
//#endif // Q_OS_WIN

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

  // Register meta types.
  detail::registerMetaTypes();

  // Warm up startup caches
  detail::warmUp();

  // Initialize translator
  {
    int lang = ac->language(),
        script = ac->languageScript();
    if (!lang ||
        lang == QLocale::Chinese && !script) {
      QLocale system = QLocale::system();
      lang =  system.language();
      script = system.script();
      //if (lang == QLocale::Japanese) {
      //  settings->setAnnotationLanguages( // Ban Chinese language
      //    Traits::JapaneseBit | Traits::UnknownLanguageBit |
      //    Traits::EnglishBit | Traits::KoreanBit |
      //    Traits::FrenchBit | Traits::GermanBit |
      //    Traits::ItalianBit |
      //    Traits::SpanishBit | Traits::PortugueseBit | Traits::RussianBit
      //  );
      //  settings->setAnnotationFilterEnabled(true);
      //}
      ac->setLanguage(lang, script);
    }
    TranslationManager::globalInstance()->setLocale(lang, script, false); // auto-update translator = false
    TranslationManager::globalInstance()->installCurrentTranslator(&a);
    DOUT("app language =" << lang);
  }

  // Check update.
  QString previousVersion = settings->version();
  if (previousVersion != G_VERSION) {
    DOUT("update from old version");
    bool majorUpdate = !previousVersion.startsWith(G_VERSION_MAJOR);


    settings->setAnnotationLanguages(QSet<int>());
    settings->setAnnotationFilterEnabled(false);

    //bool initial = previousVersion.isEmpty();


    QtExt::trashOrRemoveFile(G_PATH_CACHES "/" "cache.db");
    QtExt::trashOrRemoveFile(G_PATH_CACHES "/" "queue.db");

    QtExt::trashOrRemoveFile(G_PATH_CACHEDB);
    QtExt::trashOrRemoveFile(G_PATH_QUEUEDB);
    QtExt::trashOrRemoveFile(G_PATH_DEBUG);

#ifdef Q_OS_WIN
    if (majorUpdate) {
      // Delete plugins/*.dat
      QDir plugins(QCoreApplication::applicationDirPath() + "/plugins");
      Q_ASSERT(plugins.exists());
      foreach (const QFileInfo &fi, plugins.entryInfoList(QStringList() << "*.dat", QDir::Files)) {
        DOUT("remove old plugins caches:" << fi.absoluteFilePath());
        QFile::remove(fi.absoluteFilePath());
      }
    }
#endif // Q_OS_WIN

    if (!QFile::exists(G_PATH_CACHES "/history.xml"))
      QFile::rename(G_PATH_CACHES "/history.xml", G_PATH_MEDIADB);

#ifdef Q_OS_WIN
    WindowsRegistry::globalInstance()->unregisterRawType("DVD");
    WindowsRegistry::globalInstance()->unregisterRawType("AudioCD");
    WindowsRegistry::globalInstance()->registerShell("DVD");
#endif // Q__WS_WIN

    ac->setMenuThemeEnabled(false); // disable aero context menu effect, unless I combine qcolorization effect with my haloeffect
    ac->setThemeId(AcUi::CyanTheme);
    ac->sync();

    settings->setAnnotationFontFamily(QString());
    settings->setAnnotationJapaneseFontFamily(QString());
    settings->setAnnotationChineseFontFamily(QString());
    settings->setAnnotationPositionResolution(0);

    settings->setWindowOnTop(false);
    settings->setMultipleWindowsEnabled(false);
    settings->setAutoSubmit(true);
    settings->setAnnotationScale(1.0);
    settings->setPreferMotionlessAnnotation(true);
    settings->setAnnotationAvatarVisible(false);
    settings->setAnnotationPositionResolution(0);
    settings->setAnnotationEffect(0);
    settings->setAnnotationOffset(0);
    settings->setSubtitleColor(0);
    settings->setApplicationFilePath(QString());
    //settings->setAnnotationLanguages(Traits::AllLanguages);
    //settings->setAnnotationFilterEnabled(false);

    //settings->setAnnotationLanguages(
    //  settings->annotationLanguages() |
    //  Traits::FrenchBit | Traits::GermanBit |
    //  Traits::ItalianBit |
    //  Traits::SpanishBit | Traits::PortugueseBit | Traits::RussianBit
    //);

    settings->setLive(false);

    if (majorUpdate) {
      settings->setSaturation(1.1*1.1*1.1);
      settings->setGamma(1/1.1);
      settings->setHue(2+2+2);
      settings->setContrast(1.1);
      settings->setBrightness(1.02*1.02);

      detail::registerAssociations();
      settings->setApplicationFilePath(QCoreApplication::applicationFilePath());
    }

    settings->setVersion(G_VERSION);
    settings->sync();
  }

  // Moved
  if (settings->applicationFilePath() != QCoreApplication::applicationFilePath()) {
    detail::repairAssociations();
    settings->setApplicationFilePath(QCoreApplication::applicationFilePath());
    settings->sync();
  }

  if (ac->version() != AC_VERSION) {
    DOUT("update from old ac version");
    ac->setVersion(AC_VERSION);
  }

  // Hashes
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

    ws->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
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

  // Set translate network cache directory, same as web settings' offline cache
  TranslatorSettings::globalSettings()->setCacheDirectory(G_PATH_CACHES);

  // Set annotation cache directory, shared across different annot apps
  AnnotationCacheManager::globalInstance()->setLocation(AC_PATH_CACHES);

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

  // Allocate more threads
  enum { MinThreadCount = 3 };
  if (QThreadPool::globalInstance()->maxThreadCount() < MinThreadCount)
    QThreadPool::globalInstance()->setMaxThreadCount(MinThreadCount);
  DOUT("thread pool size =" << QThreadPool::globalInstance()->maxThreadCount());

  // Reduce animation timing interval to improve animation rendering performance
  // DEFAULT_TIMING_INTERVAL defined in qabstractanimation.cpp is 16
  // As 1 sec / 24 frames = 41.6 msec, timing interval should be at lease 41 msec
  //enum { AnimationTimingInterval = 40 };
  //QtSettings::globalInstance()->setAnimationTimingInterval(AnimationTimingInterval);

//#ifdef AC_ENABLE_GAME
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
//#endif // AC_ENABLE_GAME
  DOUT("create mainwindow");
  MainWindow w(unique); {
    DOUT("mainwindow created");
    w.setWindowTitle(TR(T_TITLE_PROGRAM));
    //Q_ASSERT(w.isValid());
    //if (!w.isValid()) {
    //  DOUT("FATAL ERROR: failed to initialize MainWindow, please contact " G_EMAIL);
    //  return -1;
    //}

//#ifdef WITH_WIN_TEXTHOOK
//    TextHook::globalInstance()->setParentWinId(w.winId());
//#endif // WITH_WIN_TEXTHOOK

    a.setMainWindow(&w);

    // Show main window
    DOUT("show mainwindow");

    QStringList args = a.arguments();

    bool showLibrary = args.size() <= 1 && QFile::exists(G_PATH_MEDIADB) && settings->showLibrary();
    w.resize(showLibrary ? LIBRARY_WINDOW_SIZE : INIT_WINDOW_SIZE);
    if (showLibrary)
      w.prepareLibrary();
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

    if (args.size() > 1) {
      args.removeFirst();
      w.openSources(args);
    }
  }

  // Do this after args are parsed by mainwindow so that annot-player works well from command line
#ifdef Q_OS_WIN
  QDir::setCurrent(QCoreApplication::applicationDirPath());
#endif // Q_OS_WIN

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
  //qMemSet(&xev, 0, sizeof(xev));
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

#if defined(AC_ENABLE_GAME) && defined(Q_OS_WIN)
  // jichi 11/29/2011: Used as a PERSISTENT hidden top level window.
  QWidget dummy;
  dummy.resize(QSize()); // zero-sized to be hidden
  QObject::connect(&w, SIGNAL(windowClosed()), &dummy, SLOT(close()));

  dummy.show();

#ifdef WITH_WIN_TEXTHOOK
  TextHook::globalInstance()->setParentWinId(dummy.winId());
  TextHook::globalInstance()->setInterval(500); // Esential!
  //TextHook::globalInstance()->start();
#endif // WITH_WIN_TEXTHOOK
#endif // AC_ENABLE_GAME && Q_OS_WIN

  //QWidget bk;
  //bk.setWindowFlags(Qt::CustomizeWindowHint);
  //DWM_ENABLE_AERO_WIDGET(&bk);
  //bk.showMaximized();

  // CHECKPOINT
  //module_t *m = module_find("freetype");;
  //qDebug()<<111111<< m->b_loaded<< m->b_unloadable;
    //FcInit();
//  qDebug()<<111111;
//  FcConfig *fcConfig = FcInitLoadConfig();
//  FcConfigBuildFonts(fcConfig);
//  qDebug()<<22222;
//#ifdef Q_OS_MAC
//    // By default, scan only the directory /System/Library/Fonts.
//    // So build the set of available fonts under another directories,
//    // and add the set to the current configuration.
//    FcConfigAppFontAddDir(NULL, "~/Library/Fonts" );
//    FcConfigAppFontAddDir(NULL, "/Library/Fonts" );
//    FcConfigAppFontAddDir(NULL, "/Network/Library/Fonts" );
//    //FcConfigAppFontAddDir( NULL, "/System/Library/Fonts" );
//    // By default, scan only the directory /System/Library/Fonts.
//#endif // Q_OS_MAC

  DOUT("exit: exec");
  return a.exec();
}

// EOF
