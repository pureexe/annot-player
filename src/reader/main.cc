// main.cc
// 6/30/2011
#include "application.h"
//#include "mainwindow.h"
#include "settings.h"
//#include "annotationsettings.h"
#include "global.h"
//#include "gamelibrary.h"
//#include "annotationgraphicsitem.h"
#include "lib/translator/translatorsettings.h"
#include "lib/mecab/mecabsettings.h"
#ifdef Q_OS_WIN
# include "registry.h"
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
#include "src/common/acversion.h"
#include "src/common/acui.h"
#include "src/common/acglobal.h"
#include "src/common/acsettings.h"
#include "src/common/acreader.h"
#include "src/common/aclocalizer.h"
#include "qtx/qxfs.h"
//#include "lib/annotdb/annotdbdefs.h"
//#include "lib/annotcloud/user.h"
//#include "lib/annotcache/annotationcachemanager.h"
//#include "lib/qt/qtsettings.h"
//#include "lib/translator/translatormanager.h"
//#include "lib/download/downloader.h"
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
#include "qtx/qxdebug.h"

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
    Registry *reg = Registry::globalInstance();
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
    Registry *reg = Registry::globalInstance();
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
    /*
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
    */
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
  a.loadTranslations();

// Delayed after args are parsed
//#ifdef Q_OS_WIN
//  QDir::setCurrent(QCoreApplication::applicationDirPath());
//#endif // Q_OS_WIN

  Settings *settings = Settings::globalSettings();
  AcSettings *ac = AcSettings::globalSettings();

  bool unique = a.isSingleInstance();
  if (!unique) {
    QStringList args = a.arguments();
    args.removeFirst();
    if (!args.isEmpty()) {
      AcReader delegate;
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
  //{
  //  int lang = ac->language(),
  //      script = ac->languageScript();
  //  if (!lang ||
  //      lang == QLocale::Chinese && !script) {
  //    QLocale system = QLocale::system();
  //    lang =  system.language();
  //    script = system.script();
  //    //if (lang == QLocale::Japanese) {
  //    //  settings->setAnnotationLanguages( // Ban Chinese language
  //    //    Traits::JapaneseBit | Traits::UnknownLanguageBit |
  //    //    Traits::EnglishBit | Traits::KoreanBit |
  //    //    Traits::FrenchBit | Traits::GermanBit |
  //    //    Traits::ItalianBit |
  //    //    Traits::SpanishBit | Traits::PortugueseBit | Traits::RussianBit
  //    //  );
  //    //  settings->setAnnotationFilterEnabled(true);
  //    //}
  //    ac->setLanguage(lang, script);
  //  }
  //  Localizer::globalInstance()->setLocale(lang, script, false); // auto-update translator = false
  //  Localizer::globalInstance()->installCurrentTranslator(&a);
  //  DOUT("app language =" << lang);
  //}

  // Check update.
  /*
  QString previousVersion = settings->version();
  if (previousVersion != G_VERSION) {
    DOUT("update from old version");
    //bool majorUpdate = !previousVersion.startsWith(G_VERSION_MAJOR);

    qreal previousVersionNumber = AcVersion::toNumber(previousVersion);

    //bool initial = previousVersion.isEmpty();

    qxTrashOrRemoveFile(G_PATH_DEBUG);

    if (previousVersionNumber < AcVersion::toNumber(ANNOTDB_VERSION)) {
      qxTrashOrRemoveFile(G_PATH_CACHEDB);
      qxTrashOrRemoveFile(G_PATH_QUEUEDB);
    }

    if (previousVersionNumber < AcVersion::toNumber(GAME_LIBRARY_VERSION))
      qxTrashOrRemoveFile(G_PATH_GAMEDB);

    if (previousVersionNumber < AcVersion::toNumber(MEDIA_LIBRARY_VERSION))
      qxTrashOrRemoveFile(G_PATH_MEDIADB);

//#ifdef Q_OS_WIN
//    if (majorUpdate) {
//      // Delete plugins/ *.dat
//      QDir plugins(QCoreApplication::applicationDirPath() + "/plugins");
//      Q_ASSERT(plugins.exists());
//      foreach (const QFileInfo &fi, plugins.entryInfoList(QStringList() << "*.dat", QDir::Files)) {
//        DOUT("remove old plugins caches:" << fi.absoluteFilePath());
//        QFile::remove(fi.absoluteFilePath());
//      }
//    }
//#endif // Q_OS_WIN

    if (previousVersionNumber < AcVersion::toNumber("0.1.9.4")) {
      settings->setAnnotationScale(ANNOTATION_SCALE);
      settings->setAnnotationFullscreenScale(ANNOTATION_FULLSCREEN_SCALE);
      settings->setTranslationServices(TranslatorManager::OcnBit);
    }
    if (previousVersionNumber < AcVersion::toNumber("0.1.9.3")) {
      ac->setMenuThemeEnabled(false); // disable aero context menu effect, unless I combine qcolorization effect with my haloeffect
      ac->setThemeId(AcUi::CyanTheme);

      settings->setAnnotationLanguages(QSet<int>());
      settings->setAnnotationFilterEnabled(false);
      settings->setAnnotationSpeedup(1.0);

      settings->setAnnotationFontFamily(QString());
      settings->setAnnotationJapaneseFontFamily(QString());
      settings->setAnnotationChineseFontFamily(QString());
      settings->setAnnotationPositionResolution(0);
      settings->setAnnotationOutlineColor(ANNOTATION_OUTLINE_COLOR);
      settings->setSubtitleOutlineColor(ANNOTATION_OUTLINE_COLOR_SUB);

      settings->setTranslateEnabled(true);
      settings->setWindowOnTop(false);
      settings->setMultipleWindowsEnabled(false);
      settings->setAutoSubmit(true);
      settings->setSubtitleOnTop(true);
      //settings->setPreferFloatAnnotation(true);
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

      settings->setSaturation(1.1*1.1*1.1);
      settings->setGamma(1/1.1);
      settings->setHue(2+2+2);
      settings->setContrast(1.1);
      settings->setBrightness(1.02*1.02);

#ifdef Q_OS_WIN
      Registry::globalInstance()->unregisterRawType("DVD");
      Registry::globalInstance()->unregisterRawType("AudioCD");
      Registry::globalInstance()->registerShell("DVD");
#endif // Q__WS_WIN
      detail::registerAssociations();
      settings->setApplicationFilePath(QCoreApplication::applicationFilePath());
    }

    ac->setVersion(AC_VERSION);
    ac->sync();
    settings->setVersion(G_VERSION);
    settings->sync();
  }

  // Moved
  if (settings->applicationFilePath() != QCoreApplication::applicationFilePath()) {
    detail::repairAssociations();
    settings->setApplicationFilePath(QCoreApplication::applicationFilePath());
    settings->sync();
  }
  */

  // Hashes
  //qDebug() << qHash(QString("hook"));
  //qDehhbug() << qHash(QString("\\wrap"));
  //qDebug() << qHash(QString("\\nowrap"));
  //qDebug() << qHash(QString("docomo"));
  //qDebug() << qHash(QString("ue"));

  // Load persistant settings

  // Load translations
  //QTranslator t;
  //bool ok = ::setTranslator(t);
  //Q_ASSERT(ok);
  //if (ok && !t.isEmpty())
  //  a.installTranslator(&t);

//#ifdef WITH_LIB_WEBBROWSER
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
//#endif // WITH_LIB_WEBBROWSER

  // Set translate network cache directory, same as web settings' offline cache
  //TranslatorSettings::globalSettings()->setCacheDirectory(G_PATH_CACHES);
  //TranslatorSettings::globalSettings()->setCacheSize(10);

  // Set annotation cache directory, shared across different annot apps
  //AnnotationCacheManager::globalInstance()->setLocation(AC_PATH_CACHES);

  // MeCab
  //MeCabSettings::setMeCabRcFile(G_PATH_MECABRC);
  //DOUT("mecabrc =" << MeCabSettings::getMeCabRcFile());

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

  // Increase animation timing interval to reduce CPU usage
  // DEFAULT_TIMING_INTERVAL defined in qabstractanimation.cpp is 16
  // As 1 sec / 24 frames = 41.6 msec, timing interval should be at lease 41 msec
  enum { AnimationTimingInterval = 32 };
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
//#endif // AC_ENABLE_GAME
  //DOUT("create mainwindow");
  //MainWindow w(unique); {
  //  DOUT("mainwindow created");
  //  w.setWindowTitle(TR(T_TITLE_PROGRAM));
  //  //Q_ASSERT(w.isValid());
  //  //if (!w.isValid()) {
  //  //  DOUT("FATAL ERROR: failed to initialize MainWindow, please contact " G_EMAIL);
  //  //  return -1;
  //  //}
//
  //  a.setMainWindow(&w);
  //}
  //DOUT("launch mainwindow");
  //w.launch(a.arguments());


  // Do this after args are parsed by mainwindow so that annot-player works well from command line
//#ifdef Q_OS_WIN
  QDir::setCurrent(QCoreApplication::applicationDirPath());
//#endif // Q_OS_WIN

#ifdef AC_ENABLE_LAUNCHER
  // Rename lauchers
  AcLocalizer::updateLocations();
#endif // AC_ENABLE_LAUNCHER

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

  //::dlpost("http://www.excite.co.jp/world/english", QUrl("http://honyaku.yahoo.co.jp/transtext"), data.toLocal8Bit());

  //w.show();
  DOUT("exit: exec");
  return a.exec();
}

// EOF
