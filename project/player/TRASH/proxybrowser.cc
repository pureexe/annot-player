// proxybrowser.cc
// 3/14/2012

#include "proxybrowser.h"
#include "global.h"
#include "tr.h"
#include "logger.h"
#include "settings.h"
#include "rc.h"
#include "ac/acui.h"
#include "ac/acsettings.h"
#include "module/nicoutil/nicoutil.h"
#include "module/qtext/algorithm.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <QtWebKit>

using namespace Logger;

#define HOMEPAGE_URL    "http://ch.nicovideo.jp/menu/anime/"

// - Construction -

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

ProxyBrowser::ProxyBrowser(QWidget *parent)
  : Base(parent)
{
  setWindowFlags(WINDOW_FLAGS);
  setWindowTitle(tr("Annot Browser"));
  setWindowIcon(QIcon(RC_IMAGE_BROWSER));
  setTextSizeMultiplier(TEXT_SIZE_SCALE);

#ifdef Q_WS_MAC
  // void setContentsMargins(int left, int top, int right, int bottom);
  setContentsMargins(4, 2, 4, 2);
#endif // Q_WS_MAC

  setHomePage(HOMEPAGE_URL);

  AcUi::globalInstance()->setWindowStyle(this);
  AcUi::globalInstance()->setStatusBarStyle(statusBar());

  setWindowOpacity(1.0); // window is opaque

  login();
}

QStringList
ProxyBrowser::startupUrls()
{
  QStringList ret = Settings::globalSettings()->browserUrls();
  if (ret.isEmpty())
    ret.append(homePage());
  else
    showMessage(tr("restoring last sessions ..."));
  return ret;
}

void
ProxyBrowser::login()
{
  AcSettings *s = AcSettings::globalSettings();
  QString userName, password;
  boost::tie(userName, password) = s->nicovideoAccount();
  if (!userName.isEmpty() && !password.isEmpty()) {
    log(tr("logging in nicovideo.jp as %1 ...").arg(userName));
    showMessage(tr("logging in nicovideo.jp as %1 ...").arg(userName));
    nico::login(userName, password, cookieJar());
  }
}

// - Events -

void
ProxyBrowser::closeEvent(QCloseEvent *event)
{
  if (tabCount() > 1) {
    QStringList urls;
    foreach (QString url, tabAddresses())
      if (!url.trimmed().isEmpty())
        urls.append(url);
    urls = QtExt::uniqueList(urls);
    Settings::globalSettings()->setBrowserUrls(urls);
  } else
    Settings::globalSettings()->clearBrowserUrls();
  Base::closeEvent(event);
}

void
ProxyBrowser::setVisible(bool visible)
{
  Base::setVisible(visible);

  if (visible && tabCount() <= 0) {
    openUrls(startupUrls());
    Settings::globalSettings()->clearBrowserUrls();
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
