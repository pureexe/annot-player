// mainwindow.cc
// 3/14/2012

#include "mainwindow.h"
#include "mainwindow_p.h"
#include "settings.h"
#include "rc.h"
#include "ac/acsettings.h"
#include "ac/acui.h"
#include "ac/acbrowser.h"
#include "ac/acplayer.h"
#include "ac/acdownloader.h"
#include "module/nicoutil/nicoutil.h"
#include "module/qtext/algorithm.h"
#include "module/qtext/network.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <QtWebKit>

#define HOMEPAGE_JP    "http://ch.nicovideo.jp/menu/anime"
#define HOMEPAGE_ZH    "http://www.bilibili.tv/video/bangumi.html"

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
#  define TEXT_SIZE_SCALE 0.9
#else
#  define TEXT_SIZE_SCALE 1.0
#endif // Q_WS_MAC

MainWindow::MainWindow(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Annot Browser"));
  setWindowIcon(QIcon(RC_IMAGE_APP));
  setWindowFlags(WINDOW_FLAGS);
  setTextSizeMultiplier(TEXT_SIZE_SCALE);

  login();

#ifdef Q_WS_MAC
  // void setContentsMargins(int left, int top, int right, int bottom);
  setContentsMargins(4, 2, 4, 2);
#endif // Q_WS_MAC

  if (AcSettings::globalSettings()->language() == QLocale::Chinese)
    setHomePage(HOMEPAGE_ZH);
  else
    setHomePage(HOMEPAGE_JP);
  setHomePages(QStringList()
    << "google.com"
    << "nicovideo.jp"
    << "ch.nicovideo.jp/menu/anime"
    << "akabeesoft2.com"
    << "syangrila.com"
    << "www.light.gr.jp"
    << "erogamescape.dyndns.org/~ap2/ero/toukei_kaiseki"
    << "erogamescape.dyndns.org/~ap2/ero/toukei_kaiseki/index_toukei.php"
  );

  setBlockedUrls(QList<QUrl>()
    << QString("http://cpro.baidu.com/")
    << QString("http://wangmeng.baidu.com/")
    << QString("http://static.loli.my/ad-images/")
  );

  AcUi::globalInstance()->setWindowStyle(this);
  AcUi::globalInstance()->setStatusBarStyle(statusBar());
  setWindowOpacity(1.0); // window is opaque

  // - IPC -
  connect(this, SIGNAL(openUrlWithAcPlayerRequested(QString)), SLOT(openUrlWithAcPlayer(QString)));
  connect(this, SIGNAL(openUrlWithAcDownloaderRequested(QString)), SLOT(openUrlWithAcDownloader(QString)));
  /*
  connect(AcBrowserController::globalController(), SIGNAL(message(QString)),
          SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(error(QString)),
          SLOT(error(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(warning(QString)),
          SLOT(warn(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(notification(QString)),
          SLOT(notify(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(arguments(QStringList)),
          SLOT(openUrls(QStringList)), Qt::QueuedConnection);
  */

  // - Shortcuts -
#ifndef Q_WS_MAC
  QShortcut *newShortcut = new QShortcut(QKeySequence::New, this);
  connect(newShortcut, SIGNAL(activated()), SLOT(newWindow()));
#endif // Q_WS_MAC

  // - Settings -
  Settings *settings = Settings::globalSettings();
  setClosedUrls(settings->closedUrls());
  addRecentUrls(settings->recentUrls());
}

QStringList
MainWindow::startupUrls()
{
  QStringList ret = Settings::globalSettings()->recentTabs();
  if (ret.isEmpty()) {
    if (AcSettings::globalSettings()->language() == QLocale::Chinese) {
      ret.append(HOMEPAGE_ZH);
      ret.append(HOMEPAGE_JP);
    } else {
      ret.append(HOMEPAGE_JP);
      ret.append(HOMEPAGE_ZH);
    }
  } else
    showMessage(tr("restoring last sessions ..."));
  return ret;
}

void
MainWindow::login()
{
  DOUT("enter");
  QString userName, password;
  boost::tie(userName, password) = AcSettings::globalSettings()->bilibiliAccount();
  if (!userName.isEmpty() && !password.isEmpty()) {
    showMessage(tr("logging in bilibili.tv as %1 ...").arg(userName));
    bilibili::login(userName, password, cookieJar());
  }
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
      QFileOpenEvent *fe = static_cast<QFileOpenEvent *>(e);
      Q_ASSERT(fe);
      QString url = fe->url().toString();
      if (!url.isEmpty())
        QTimer::singleShot(0, new slot_::OpenUrls(QStringList(url), this), SLOT(openUrls()));
    } break;
  default: accept = Base::event(e);
  }
  return accept;
}

void
MainWindow::saveRecentTabs()
{
  if (tabCount() > 1) {
    QStringList urls;
    foreach (QString url, tabAddresses())
      if (!url.trimmed().isEmpty())
        urls.append(url);
    urls = QtExt::uniqueList(urls);
    Settings::globalSettings()->setRecentTabs(urls);
  } else
    Settings::globalSettings()->clearRecentTabs();
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
  DOUT("enter");
  enum { UrlSizeLimit = 20 };

  // Save settings
  saveRecentTabs();
  Settings *settings = Settings::globalSettings();
  settings->setClosedUrls(closedUrls(), UrlSizeLimit);
  settings->setRecentUrls(recentUrls(), UrlSizeLimit);

  AcBrowserController::globalController()->stop();
  AcPlayerController::globalController()->stop();
  AcDownloaderController::globalController()->stop();

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
    Settings::globalSettings()->clearRecentTabs();
  }
}

// - New window -

void
MainWindow::newWindow()
{
  bool ok = false;
  QString app = QCoreApplication::applicationFilePath();
#ifdef Q_WS_MAC
  Q_UNUSED(app);
  //ok = QtMac::open(app);
#elif defined Q_WS_WIN
  ok = QProcess::startDetached('"' + app + '"');
#else
  ok = QProcess::startDetached(app);
#endif // Q_WS_
  if (ok)
    showMessage(tr("openning new window") + " ...");
  else
    warn(tr("failed open new window"));
}

// - IPC -

void
MainWindow::openUrlWithAcPlayer(const QString &address)
{
  DOUT("enter: address =" << address);
  QString url = address.trimmed();
  if (url.isEmpty())
    return;
  if (!url.startsWith("http://"))
    url.prepend("http://");
  showMessage(tr("openning") + " " + url);
  AcPlayerController::globalController()->openUrl(url);
  DOUT("exit");
}

void
MainWindow::openUrlWithAcDownloader(const QString &address)
{
  DOUT("enter: address =" << address);
  QString url = address.trimmed();
  if (url.isEmpty())
    return;
  if (!url.startsWith("http://", Qt::CaseInsensitive))
    url.prepend("http://");
  showMessage(tr("openning") + " " + url);
  AcDownloaderController::globalController()->openUrl(url);
  DOUT("exit");
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
