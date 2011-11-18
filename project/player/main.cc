// main.cc
// 6/30/2011

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
#ifdef Q_WS_WIN
  #include "win/qtwin/qtwin.h"
#endif // W_WS_WIN
#ifdef Q_WS_MAC
  #include "mac/qtmac/qtmac.h"
#endif // Q_WS_MAC
#include <QtGui>
#include <QtWebKit>

#define DEBUG "main"
#include "module/debug/debug.h"

// - Debug -

namespace { // anonymous, debug

  // See: http://www.cppblog.com/lauer3912/archive/2011/04/10/143870.html
  void
  debugMessageHandler(QtMsgType type, const char *msg)
  {
#define TIMESTAMP QDateTime::currentDateTime().toString("MM:dd: hh:mm:ss")
    QString output;
    switch (type) {
    case QtDebugMsg:    output = QString("%1: %2\n").arg(TIMESTAMP).arg(msg); break;
    case QtWarningMsg:  output = QString("%1: warning: %2\n").arg(TIMESTAMP).arg(msg); break;
    case QtCriticalMsg: output = QString("%1: critical: %2\n").arg(TIMESTAMP).arg(msg); break;
    case QtFatalMsg:    output = QString("%1: fatal: %2\n").arg(TIMESTAMP).arg(msg); break;
    }

    QFile file(G_PATH_DEBUG);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
      QTextStream(&file) << output;
#undef TIMESTAMP
  }

} // anonymous namespace

// - Meta types -

namespace { // anonymous
  inline void registerMetaTypes()
  {
    //qRegisterMetaType<WId>("WId");
  }
} // anonymous namespace

// - i18n -

/*
namespace { // anonymous

  inline bool setTranslator(QTranslator /&t)
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

} // anonymous namespace
*/

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
  // Directory
  {
#ifdef USE_MODE_DEBUG
    QDir logs(G_PATH_LOGS);
    if (!logs.exists())
      logs.mkpath(logs.absolutePath());
#endif // USE_MODE_DEBUG

    QDir caches(G_PATH_CACHES);
    if (!caches.exists())
      caches.mkpath(caches.absolutePath());
  }
  // Set global random seed.
  ::qsrand((uint)QDateTime::currentMSecsSinceEpoch());

  // Applications
  Application a(argc, argv);
#ifdef Q_WS_WIN
  {
    QFileInfo fi(QCoreApplication::applicationFilePath());
    QString processName = fi.fileName();
    //DOUT("main: looking for process, processName =" << processName);
    QList<ulong> pids = QtWin::getProcessIdsByName(processName);
    if (pids.size() > 1) {
      //DOUT("main: multple processes detected, count =" << pids.size());
      //DOUT("main:exit: ret = 0");
      return 0;
    }
  }
#endif // Q_WS_WIN

  ::registerMetaTypes();

#ifdef USE_MODE_DEBUG
  {
    QFile debug(G_PATH_DEBUG);
    if (debug.open(QIODevice::WriteOnly | QIODevice::Append)) {
      QTextStream(&debug)
        << "\n--------------------------------------------------------------------------------\n\n";
      debug.close();

      qInstallMsgHandler(debugMessageHandler);
      DOUT("main: debug message handler installed");
    }
  }
#endif // USE_MODE_DEBUG

#ifdef Q_WS_WIN
  QtWin::warmUp();
#endif // Q_WS_WIN

#ifdef USE_WIN_DWM
  Dwm::warmUp();
#endif // USE_WIN_DWM

  // Cache needed fonts in Mac OS X
  AnnotationGraphicsItem::warmUp();

  // Initialize translator
  {
    int lang = Settings::globalInstance()->language();
    if (!lang) {
      lang =  QLocale::system().language();
      Settings::globalInstance()->setLanguage(lang);
    }
    TranslatorManager::globalInstance()->setLanguage(lang, false); // auto-update translator = false
    TranslatorManager::globalInstance()->installCurrentTranslator(&a);
    DOUT("main: app language =" << lang);
  }

  Settings::globalInstance()->setVersion(G_VERSION);

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
    int tid = Settings::globalInstance()->themeId();
    UiStyle::globalInstance()->setTheme(tid);
  }

#ifdef USE_MODE_SIGNAL
  // Root window
  QMainWindow root; // Persistant visible root widget to prevent Qt from automatic closing invisible windows
  root.setWindowFlags(root.windowFlags() | Qt::WindowStaysOnTopHint);
  root.resize(0, 0);

  // Main window
  MainWindow w(&root, WINDOW_FLAGS);
  root.show();
#else
  MainWindow w;

#endif // USE_MODE_SIGNAL
  Q_ASSERT(w.isValid());

#ifdef USE_WIN_QTH
  QTH->setParentWinId(w.winId());
#endif // USE_WIN_QTH

  a.setMainWindow(&w);

  // Show main window
  w.resize(INIT_WINDOW_SIZE);
  w.show();

  // Automatic login
  QString userName = Settings::globalInstance()->userName(),
          password = Settings::globalInstance()->password();
  if (!userName.isEmpty() && !password.isEmpty())
    w.login(userName, password);

  w.parseArguments(a.arguments());

  //QWidget t;
  //t.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
  //DWM_ENABLE_AERO_WIDGET((&t));
  //t.showMaximized();

  //w.openPath("d:\\i\\sample.mp4");
  //w.openPath("d:/i/sample.mp4");
  //w.openPath("file:///d:/i/sample.mp4");
  //w.openPath("/Volumes/local/i/sample.mp4");
  return a.exec();
}

// EOF
