// acwebwindow.cc
// 2/17/2012

#include "ac/acwebwindow.h"
#include "ac/acui.h"
#include "module/qtext/webview.h"
#include <QtWebKit/QWebView>

// - Construction -

AcWebWindow::AcWebWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f)
{
  AcUi::globalInstance()->setWindowStyle(this);
  setWindowOpacity(1.0);

  QtExt::WebView *w = new QtExt::WebView; {
    connect(w, SIGNAL(message(QString)), SLOT(showMessage(QString)));
    connect(w, SIGNAL(errorMessage(QString)), SLOT(error(QString)));
    connect(w, SIGNAL(warning(QString)), SLOT(warn(QString)));
    connect(w, SIGNAL(notification(QString)), SLOT(notify(QString)));
  }
  setCentralWidget(w);
}

// - Properties -

QWebView*
AcWebWindow::webView() const
{ return qobject_cast<QWebView *>(centralWidget()); }

// EOF
