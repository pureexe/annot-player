// main.cc
// 6/30/2011
#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include "defines.h"
#include "uistyle.h"
#include "processinfo.h"
#include "translatormanager.h"
#include "annotationgraphicsitem.h"
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#endif // USE_WIN_QTH
#ifdef USE_WIN_DWM
  #include "win/dwm/dwm.h"
#endif // USE_WIN_DWM
#ifdef Q_OS_WIN
  #include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
  #include "mac/qtmac/qtmac.h"
#endif // Q_OS_MAC
#include <QtGui>
#ifdef USE_MODULE_WEBBROWSER
#  include <QtWebKit>
#endif // USE_MODULE_WEBBROWSER
#include <ctime>
#include <cstdlib>

//#define DEBUG "main"
#include "module/debug/debug.h"

// - Startup stages -

namespace { // anonymous

  // Meta types
  /*
  inline void registerMetaTypes()
  {
    qRegisterMetaType<ProcessInfo>("ProcessInfo");
  }
  */

  // i18n
  /*
  inline bool setTranslator(QTranslator &t)
  {
    QString qm;
    switch (QLocale::system().language()) {
    case QLocale::English:
      qm = RC_TR_EN;
      break;
    case QLocale::Japanese:
      qm = RC_TR_JA;
      break;
    case QLocale::Chinese:
      qm = RC_TR_ZH;
      break;
    case QLocale::Taiwan:
      qm = RC_TR_TW;
      break;
    default:
      qm = RC_TR_EN;
    }

    return t.load(qm);
  }
  */

  // Warm up
  inline void warmUp()
  {
#ifdef Q_OS_WIN
    QtWin::warmUp();
#endif // Q_OS_WIN

#ifdef USE_WIN_DWM
    Dwm::warmUp();
#endif // USE_WIN_DWM

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
  // Set OS encoding to UTF-8 before application starts.
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForCStrings(codec);
  QTextCodec::setCodecForTr(codec);

  // Applications
  Application a(argc, argv);
  if (!a.isSingleInstance())
    return 0;

#ifdef USE_MODE_DEBUG
  //a.installMessageHandlers();
#endif // USE_MODE_DEBUG

  Settings *settings = Settings::globalInstance();

  // Seed global random generator.
  ::srand((uint)::time(0));
  ::qsrand((uint)QDateTime::currentMSecsSinceEpoch());

  // Register meta types.
  //::registerMetaTypes();

  // Warm up startup caches
  ::warmUp();

  // Initialize translator
  {
    int lang = settings->language();
    if (!lang) {
      lang =  QLocale::system().language();
      settings->setLanguage(lang);
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

#ifdef USE_MODULE_WEBBROWSER
  // Set webkit settings
  {
    QWebSettings *ws = QWebSettings::globalSettings();

    ws->setAttribute(QWebSettings::PluginsEnabled, true);
    ws->setAttribute(QWebSettings::AutoLoadImages, true);
    ws->setAttribute(QWebSettings::JavascriptEnabled, true);

    //g->setMaximumPagesInCache(10);
  }
#endif // USE_MODULE_WEBBROWSER

  // Set theme.
  {
    int tid = settings->themeId();
    UiStyle::globalInstance()->setTheme(tid);
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
  MainWindow w; {
    Q_ASSERT(w.isValid());
    //if (!w.isValid()) {
    //  DOUT("FATAL ERROR: failed to initialize MainWindow, please contact " G_EMAIL);
    //  return -1;
    //}

//#ifdef USE_WIN_QTH
//    QTH->setParentWinId(w.winId());
//#endif // USE_WIN_QTH

    a.setMainWindow(&w);

    // Show main window
    w.resize(INIT_WINDOW_SIZE);
    w.show();
    //QTimer::singleShot(0, &w, SLOT(show()));

    // Automatic login
    QString userName = settings->userName(),
            password = settings->password();
    if (!userName.isEmpty() && !password.isEmpty())
      w.login(userName, password);
    else
      w.checkInternetConnection();

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

#ifdef USE_WIN_QTH
  QTH->setParentWinId(dummy.winId());
  QTH->setInterval(QTH_INTERVAL);
#endif // USE_WIN_QTH

  dummy.show();
  //QTimer::singleShot(0, &dummy, SLOT(show()));

#endif // USE_MODE_SIGNAL && Q_OS_WIN

  //QWidget bk;
  //bk.setWindowFlags(Qt::CustomizeWindowHint);
  //DWM_ENABLE_AERO_WIDGET(&bk);

  //bk.showMaximized();

  //QString url = "http://smile-com00.nicovideo.jp/smile?m=17054368.25318";
  //QRegExp rx("http://([\\w\\d\\-]+)\\.nicovideo\\.jp/smile\\?m=([\\w\\d\\.]+)", Qt::CaseInsensitive);
  //if (rx.exactMatch(url)) {
  //  qDebug()<<rx.captureCount();
  //  qDebug()<<rx.cap(1);
  //  qDebug()<<rx.cap(2);
  //}

  return a.exec();
}

// EOF
