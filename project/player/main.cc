// main.cc
// 6/30/2011

#include "mainwindow.h"
#include "global.h"
#include "rc.h"
#include "translator.h"
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#endif // USE_WIN_QTH
#include <QtGui>
#include <QtWebKit>

// - i18n -

/*
namespace { // anonymous

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

  QApplication a(argc, argv);
  Translator::globalInstance()->installCurrentTranslator(&a);

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

#ifdef USE_MODE_SIGNAL
  // Root window
  QMainWindow root; // Persistant visible root widget to prevent Qt from automatic closing invisible windows
  root.setWindowFlags(root.windowFlags() | Qt::WindowStaysOnTopHint);
  root.resize(0,0);

  // Main window
  MainWindow w(&root, WINDOW_FLAGS);
  root.show();
#else
  MainWindow w;
#endif // USE_MODE_SIGNAL
  Q_ASSERT(w.isValid());
  w.resize(INIT_WINDOW_SIZE);
  w.show();
#ifdef USE_WIN_QTH
  QTH->setParentWinId(w.winId());
#endif // USE_WIN_QTH

  w.parseArguments(a.arguments());

  //w.openPath("d:\\i\\sample.mp4");
  //w.openPath("d:/i/sample.mp4");
  //w.openPath("file:///d:/i/sample.mp4");
  //w.openPath("/Volumes/local/i/sample.mp4");

  return a.exec();
}

// EOF

