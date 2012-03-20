#ifndef PROXYBROWSER_H
#define PROXYBROWSER_H

// proxybrowser.h
// 3/14/2012

#include <module/webbrowser/webbrowser.h>

class ProxyBrowser: public WebBrowser
{
  Q_OBJECT
  typedef ProxyBrowser Self;
  typedef WebBrowser Base;

public:
  explicit ProxyBrowser(QWidget *parent = 0);

public slots:
  void login();

  virtual void setVisible(bool visible); ///< \override
protected:
  virtual void closeEvent(QCloseEvent *event); ///< \override

  static QStringList startupUrls();
};

#endif // PROXYBROWSER_H
