// mainwindow.cc
// 3/14/2012

#include "mainwindow.h"
#include "settings.h"
#include "rc.h"
#include "ac/acsettings.h"
#include "ac/acui.h"
#include "module/nicoutil/nicoutil.h"
#include "module/qtext/algorithm.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <QtWebKit>

#define HOMEPAGE_URL    "http://ch.nicovideo.jp/menu/anime/"

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

  AcUi::globalInstance()->setWindowStyle(this);
  setWindowOpacity(1.0); // window is opaque

  login();
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
  AcSettings *s = AcSettings::globalSettings();
  QString userName, password;
  boost::tie(userName, password) = s->nicovideoAccount();
  if (!userName.isEmpty() && !password.isEmpty()) {
    showMessage(tr("logging in nicovideo.jp as %1 ...").arg(userName));
    nico::login(userName, password, cookieJar());
  }
}

// - Events -

void
MainWindow::closeEvent(QCloseEvent *event)
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
  Base::closeEvent(event);
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
