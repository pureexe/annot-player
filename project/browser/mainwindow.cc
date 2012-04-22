// mainwindow.cc
// 3/14/2012
#include "mainwindow.h"
#include "mainwindow_p.h"
#include "settings.h"
#include "rc.h"
#include "global.h"
#include "ac/acsettings.h"
#include "ac/acui.h"
#include "ac/acbrowser.h"
#include "ac/acplayer.h"
#include "ac/acdownloader.h"
#include "module/webbrowser/network/wbnetworkaccessmanager.h"
#include "module/nicoutil/nicoutil.h"
#include "module/download/downloader.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include "module/qtext/algorithm.h"
#include "module/qtext/network.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <QtWebKit>

#define HOMEPAGE_EN    "http://www.youtube.com/FUNimation"
#define HOMEPAGE_JP    "http://ch.nicovideo.jp/menu/anime"
#define HOMEPAGE_ZH    "http://www.bilibili.tv/video/bangumi.html"

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

// - Construction -

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

#ifdef Q_WS_MAC
  // void setContentsMargins(int left, int top, int right, int bottom);
  setContentsMargins(4, 2, 4, 2);
#endif // Q_WS_MAC

  loadCookieJar();

  setCacheDirectory(G_PATH_CACHES);

  setBlockedUrls(QList<QUrl>()
    << QString("http://wangmeng.baidu.com")
    << QString("http://cpro.baidu.com")
    << QString("http://static.loli.my/ad-images")
    << QString("http://u17.com")
    << QString("http://u17i.com")
    << QString("http://taobao.com")
    << QString("http://www.bilibili.tv/html/arcgg.html")
  );

  switch (AcSettings::globalSettings()->language()) {
  case QLocale::English: setHomePage(HOMEPAGE_JP); break;
  case QLocale::Chinese: setHomePage(HOMEPAGE_ZH); break;
  default:               setHomePage(HOMEPAGE_JP);
  }
  setHomePages(QStringList()
    << "google.com"
    << "nicovideo.jp"
    << "ch.nicovideo.jp/menu/anime"
    << "www.light.gr.jp"
    << WbNetworkAccessManager::supportedSites()
  );

  AcUi::globalInstance()->setWindowStyle(this);
  AcUi::globalInstance()->setStatusBarStyle(statusBar());
  setWindowOpacity(1.0); // window is opaque

  // - IPC -
  playerDelegate_ = new AcPlayer(this);
  downloaderDelegate_ = new AcDownloader(this);
  connect(this, SIGNAL(openUrlWithAcPlayerRequested(QString)), SLOT(openUrlWithAcPlayer(QString)));
  connect(this, SIGNAL(importUrlToAcPlayerRequested(QString)), SLOT(importUrlToAcPlayer(QString)));
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

  // - Message -
  DownloaderController *dc = DownloaderController::globalController();
  connect(dc, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(error(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);

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

void
MainWindow::showMessage(const QString &text)
{
  if (MrlAnalysis::matchSite(text))
    notify(text);
  else
    Base::showMessage(text);
}

QStringList
MainWindow::startupUrls()
{
  QStringList ret = Settings::globalSettings()->recentTabs();
  if (!ret.isEmpty())
    showMessage(tr("restoring last sessions ..."));
  //else
  //  switch (AcSettings::globalSettings()->language()) {
  //  case QLocale::Chinese:
  //    ret.append(HOMEPAGE_ZH);
  //    ret.append(HOMEPAGE_JP);
  //    break;
  //  case QLocale::Japanese:
  //    ret.append(HOMEPAGE_JP);
  //    ret.append(HOMEPAGE_ZH);
  //    break;
  //  default:
  //    ret.append(HOMEPAGE_EN);
  //    ret.append(HOMEPAGE_JP);
  //  }
  //}
  return ret;
}

void
MainWindow::login()
{
  DOUT("enter");
  QString userName, password;
  //boost::tie(userName, password) = AcSettings::globalSettings()->bilibiliAccount();
  //if (!userName.isEmpty() && !password.isEmpty()) {
  //  showMessage(tr("logging in bilibili.tv as %1 ...").arg(userName));
  //  bilibili::login(userName, password, cookieJar());
  //}
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
      QString url = fe->file();
      if (!url.isEmpty())
        url.prepend("file:///");
      else
        url = fe->url().toString();
      if (!url.isEmpty())
        QTimer::singleShot(0, new slot_::NewTab(url, this), SLOT(trigger()));
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
    foreach (QString t, tabAddresses()) {
      t = t.trimmed();
      if (!t.isEmpty() && t != "about:blank")
        urls.append(t);
    }
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
  settings->setClosedUrls(QtExt::skipEmpty(closedUrls()), UrlSizeLimit);
  settings->setRecentUrls(QtExt::skipEmpty(recentUrls()), UrlSizeLimit);
  settings->setRecentTabIndex(tabIndex());
  settings->setRecentSize(size());

  DownloaderController::globalController()->abort();

  saveCookieJar();

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
    Settings *s = Settings::globalSettings();

    openUrls(startupUrls());
    focusTab(s->recentTabIndex());
    s->clearRecentTabs();
    s->setRecentTabIndex(0);

    if (tabCount() <= 0)
      newTabWithDefaultPage();
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
  playerDelegate_->openUrl(url);
  DOUT("exit");
}

void
MainWindow::importUrlToAcPlayer(const QString &address)
{
  DOUT("enter: address =" << address);
  QString url = address.trimmed();
  if (url.isEmpty())
    return;
  if (!url.startsWith("http://"))
    url.prepend("http://");
  showMessage(tr("openning") + " " + url);
  playerDelegate_->importUrl(url);
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
  downloaderDelegate_->openUrl(url);
  DOUT("exit");
}

// - Caches -

void
MainWindow::loadCookieJar()
{
  bool ok = QtExt::readCookiesfromFile(cookieJar(), G_PATH_COOKIE);
  Q_UNUSED(ok);
  DOUT("ok =" << ok);
}

void
MainWindow::saveCookieJar()
{
  bool ok = QtExt::writeCookiesToFile(cookieJar(), G_PATH_COOKIE);
  Q_UNUSED(ok);
  DOUT("ok =" << ok);
}

// EOF
