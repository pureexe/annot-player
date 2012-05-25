// acwebwindow.cc
// 2/17/2012

#include "project/common/acwebwindow.h"
#include "project/common/acui.h"
#include "module/qtext/webview.h"
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtGui/QShortcut>
#include <QtCore/QEvent>

#define DEBUG "acwebwindow"
#include "module/debug/debug.h"

// - Construction -

AcWebWindow::AcWebWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f)
{
  setRippleEnabled(false);
  AcUi::globalInstance()->setWindowStyle(this);
  setWindowOpacity(1.0);

  QtExt::WebView *w = new QtExt::WebView(this); {
    connect(w, SIGNAL(message(QString)), SLOT(showMessage(QString)));
    connect(w, SIGNAL(errorMessage(QString)), SLOT(error(QString)));
    connect(w, SIGNAL(warning(QString)), SLOT(warn(QString)));
    connect(w, SIGNAL(notification(QString)), SLOT(notify(QString)));
  }
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
  QNetworkDiskCache *cache = 0;
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
