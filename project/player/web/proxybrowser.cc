// proxybrowser.cc
// 3/14/2012

#include "proxybrowser.h"
#include "defines.h"
#include "uistyle.h"
#include "tr.h"
#include "settings.h"
#include "logger.h"
#include "module/nicoutil/nicoutil.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <QtWebKit>

using namespace Logger;

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

ProxyBrowser::ProxyBrowser(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Web browser"));
  setWindowFlags(WINDOW_FLAGS);
  setTextSizeMultiplier(TEXT_SIZE_SCALE);

#ifdef Q_WS_MAC
  // void setContentsMargins(int left, int top, int right, int bottom);
  setContentsMargins(4, 2, 4, 2);
#endif // Q_WS_MAC

  setHomePage("http://www.google.com");

  UiStyle::globalInstance()->setWindowStyle(this);
  setWindowOpacity(1.0); // window is opaque

  login();

  openUrls(QStringList()
    << "http://ch.nicovideo.jp/menu/anime/"
  );
}

void
ProxyBrowser::login()
{
  Settings *s = Settings::globalInstance();
  QString userName, password;
  boost::tie(userName, password) = s->nicovideoAccount();
  if (!userName.isEmpty() && !password.isEmpty()) {
    log(tr("logging in nicovideo.jp as %1 ...").arg(userName));
    nico::login(userName, password, cookieJar());
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
