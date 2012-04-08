// mainwindow.cc
// 3/14/2012

#include "mainwindow.h"
#include "mainwindowprivate.h"
#include "settings.h"
#include "rc.h"
#include "ac/acsettings.h"
#include "ac/acui.h"
#include "module/nicoutil/nicoutil.h"
#include "module/qtext/algorithm.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <QtWebKit>

#define HOMEPAGE_URL    "http://ch.nicovideo.jp/menu/anime"

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

#ifdef Q_WS_WIN
  #define TEXT_SIZE_SCALE 0.9
#else
  #define TEXT_SIZE_SCALE 1.0
#endif // Q_WS_MAC

MainWindow::MainWindow(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Annot Browser"));
  setWindowIcon(QIcon(RC_IMAGE_APP));
  setWindowFlags(WINDOW_FLAGS);
  setTextSizeMultiplier(TEXT_SIZE_SCALE);

#ifdef Q_WS_MAC
  // void setContentsMargins(int left, int top, int right, int bottom);
  setContentsMargins(4, 2, 4, 2);
#endif // Q_WS_MAC

  setHomePage(HOMEPAGE_URL);
  setHomePages(QStringList()
    << "google.com"
    << "nicovideo.jp"
    << "akabeesoft2.com"
    << "syangrila.com"
    << "www.light.gr.jp"
    << "erogamescape.dyndns.org/~ap2/ero/toukei_kaiseki"
    << "erogamescape.dyndns.org/~ap2/ero/toukei_kaiseki/index_toukei.php"
  );

  AcUi::globalInstance()->setWindowStyle(this);
  AcUi::globalInstance()->setStatusBarStyle(statusBar());
  setWindowOpacity(1.0); // window is opaque

#ifndef Q_WS_MAC
  QShortcut *n = new QShortcut(QKeySequence::New, this);
  connect(n, SIGNAL(activated()), SLOT(newWindow()));
#endif // Q_WS_MAC
}

QStringList
MainWindow::startupUrls()
{
  QStringList ret = Settings::globalSettings()->recentUrls();
  if (ret.isEmpty())
    ret.append(homePage());
  else
    showMessage(tr("restoring last sessions ..."));
  return ret;
}

void
MainWindow::login()
{
  DOUT("enter");
  QString userName, password;
  boost::tie(userName, password) = AcSettings::globalSettings()->nicovideoAccount();
  if (!userName.isEmpty() && !password.isEmpty()) {
    showMessage(tr("logging in nicovideo.jp as %1 ...").arg(userName));
    nico::login(userName, password, cookieJar());
  }
  DOUT("exit");
}

// - Events -

bool
MainWindow::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen: // See: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder
    {
      QFileOpenEvent *fe = dynamic_cast<QFileOpenEvent *>(e);
      Q_ASSERT(fe);
      if (fe) {
        QString url = fe->file();
        QTimer::singleShot(0, new slot_::OpenUrl(url, this), SLOT(openUrl()));
      }
    } break;
  default: accept = Base::event(e);
  }
  return accept;
}

void
MainWindow::saveRecentUrls()
{
  if (tabCount() > 1) {
    QStringList urls;
    foreach (QString url, tabAddresses())
      if (!url.trimmed().isEmpty())
        urls.append(url);
    urls = QtExt::uniqueList(urls);
    Settings::globalSettings()->setRecentUrls(urls);
  } else
    Settings::globalSettings()->clearRecentUrls();
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
  DOUT("enter");
  saveRecentUrls();

#ifdef Q_WS_WIN
  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
#endif // Q_WS_WIN

  Base::closeEvent(event);
  DOUT("close");
}

void
MainWindow::setVisible(bool visible)
{
  Base::setVisible(visible);

  if (visible && tabCount() <= 0) {
    openUrls(startupUrls());
    Settings::globalSettings()->clearRecentUrls();
  }
}

// - New window -

void
MainWindow::newWindow()
{
  bool ok = false;
#ifdef Q_WS_MAC
  //ok = QtMac::open("Annot Browser");
#elif defined Q_WS_WIN
  QString exe = QCoreApplication::applicationDirPath() + "/" + "annot-browser";
  ok = QProcess::startDetached('"' + exe + '"');
#else
  ok = QProcess::startDetached("annot-browser");
#endif // Q_WS_
  if (ok)
    showMessage(tr("openning new window") + " ...");
  else
    warn(tr("failed open new window"));
}
// EOF

/*
void
CloudView::setVisible(bool visible)
{
  if (visible && tabCount() == 0)
    openUrl(homePage());

  Base::setVisible(visible);
}
*/
