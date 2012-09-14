// acwebwindow.cc
// 2/17/2012

#include "src/common/acwebwindow.h"
#include "src/common/acui.h"
#include "src/common/acprotocol.h"
#include "lib/qtext/webview.h"
#include <QtCore/QEvent>
#include <QtGui/QShortcut>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebView>

#define DEBUG "acwebwindow"
#include "lib/debug/debug.h"

// - Construction -

AcWebWindow::AcWebWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f)
{
  AcUi::globalInstance()->setWindowStyle(this);
  setWindowOpacity(1.0);

  QtExt::WebView *w = new QtExt::WebView(this);
  AC_CONNECT_MESSAGES(w, this, Qt::AutoConnection);
  setCentralWidget(w);

  //grabGesture(Qt::PanGesture);
  //grabGesture(Qt::SwipeGesture);
  //grabGesture(Qt::PinchGesture);

  //new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));
  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(fadeOut()));
}

// - Properties -

void
AcWebWindow::setCacheDirectory(const QString &path)
{
  QWebView *view = webView();
  if (!view)
    return;
  QWebPage *page = view->page();
  if (!page)
    return;
  QNetworkAccessManager *nam = page->networkAccessManager();
  if (!nam)
    return;
  QNetworkDiskCache *cache = nullptr;
  if (!path.isEmpty()) {
    cache = new QNetworkDiskCache(nam);
    cache->setCacheDirectory(path);
  }
  nam->setCache(cache);
}

QWebView*
AcWebWindow::webView() const
{ return qobject_cast<QWebView *>(centralWidget()); }

// - Events -

bool
AcWebWindow::event(QEvent *event)
{
  if (event->type() == QEvent::Gesture)
    DOUT("gesture event");
  return Base::event(event);
}

// EOF
