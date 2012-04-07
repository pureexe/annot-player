#ifndef WBWEBVIEW_H
#define WBWEBVIEW_H

// wbnetwork.h
// 1/27/2012

#include "module/qtext/webdialog.h"

typedef QtExt::WebDialog WbWebViewBase;
class WbWebView : public WbWebViewBase
{
  Q_OBJECT
  typedef WbWebView Self;
  typedef WbWebViewBase Base;
public:
  explicit WbWebView(QWidget *parent = 0)
    : Base(parent) { setWindowFlags(0); }

signals:
  void windowCreated(QWebView *w);

protected:
  virtual QWebView *createWindow(QWebPage::WebWindowType type) ///< \override
  {
    if (type != QWebPage::WebBrowserWindow)
      return Base::createWindow(type);
    Self *ret = new Self(parentWidget());
    emit windowCreated(ret);
    return ret;
  }
};

#endif // WBWEBVIEW_H
