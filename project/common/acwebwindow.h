#ifndef ACWEBWINDOW_H
#define ACWEBWINDOW_H

// acwebwindow.h
// 4/4/2012

#include "project/common/acmainwindow.h"

QT_FORWARD_DECLARE_CLASS(QWebView)

class AcWebWindow : public AcMainWindow
{
  Q_OBJECT
  typedef AcWebWindow Self;
  typedef AcMainWindow Base;

  // - Construction -
public:
  explicit AcWebWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

  QWebView *webView() const; ///< central widget
};

#endif // ACWEBWINDOW_H
