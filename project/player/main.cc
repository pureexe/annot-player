// main.cc
// 6/30/2011

// 12/5/2011 TODO: clean up this file.
// Gradually move things that might be reusable into Application.

#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include "global.h"
#include "uistyle.h"
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
#include <QtWebKit>

#define DEBUG "main"
#include "module/debug/debug.h"


// - Startup stages -

namespace { // anonymous

  // Meta types
  inline void registerMetaTypes()
  {
    //qRegisterMetaType<WId>("WId");
  }

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
//#ifdef USE_WIN_QTH
//  Qth::startService();
//#endif // USE_WIN_QTH

  // Set OS encoding to UTF-8 before application starts.
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  // Applications
  Application a(argc, argv);
  if (!a.isSingleInstance())
    return 0;

#ifdef USE_MODE_DEBUG
  //a.startLoggingDebugMessage();
#endif // USE_MODE_DEBUG

  Settings *settings = Settings::globalInstance(); // Global settings
  Q_ASSERT(settings);

  // Set global random seed.
  ::qsrand((uint)QDateTime::currentMSecsSinceEpoch());

  // Register meta types.
  ::registerMetaTypes();

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
    DOUT("main: app language =" << lang);
  }

  // Rebuild caches on update.
  if (settings->version() != G_VERSION) {
    DOUT("main: update from old version");
    QFileInfo cachedb(G_PATH_CACHEDB),
              queuedb(G_PATH_QUEUEDB);
    if (cachedb.exists())
      QFile::remove(cachedb.filePath());
    if (queuedb.exists())
      QFile::remove(queuedb.filePath());
    settings->setVersion(G_VERSION);
  }

  // Hashes
  //qDebug() << qHash(QString("\\sub"));
  //qDebug() << qHash(QString("\\subtitle"));

  // Load persistant settings

  // Load translations
  //QTranslator t;
  //bool ok = ::setTranslator(t);
  //Q_ASSERT(ok);
  //if (ok && !t.isEmpty())
  //  a.installTranslator(&t);

  // Set webkit settings
  QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

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
    if (!w.isValid()) {
      DOUT("FATAL ERROR: failed to initialize MainWindow, please contact " G_EMAIL);
      return -1;
    }

#ifdef USE_WIN_QTH
    QTH->setParentWinId(w.winId());
#endif // USE_WIN_QTH

    a.setMainWindow(&w);

    // Show main window
    w.resize(INIT_WINDOW_SIZE);
    w.show();

    // Automatic login
    QString userName = settings->userName(),
            password = settings->password();
    if (!userName.isEmpty() && !password.isEmpty())
      w.login(userName, password);

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

  /*
  Display *dpy = QX11Info::display();
  XEvent xev;
  Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);

  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = w.winId();
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1;
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;

  XSendEvent(dpy, DefaultRootWindow(dpy), False,
  SubstructureNotifyMask, &xev);
  */

#if defined(USE_MODE_SIGNAL) && defined(Q_OS_WIN)
  // jichi 11/29/2011: Use as a persistent top level window.
  // FIXME: The app would close when there is no visible window after hijacking another process.
  QWidget dummy; // Dummy widget as a persistent visible zero-sized window.
  dummy.resize(QSize());
  QObject::connect(&w, SIGNAL(windowClosed()), &dummy, SLOT(close()));
  dummy.show();
#endif // USE_MODE_SIGNAL && Q_OS_WIN

  return a.exec();
}

// EOF
