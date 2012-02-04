#ifndef _QTEXT_WEBDIALOG_H
#define _QTEXT_WEBDIALOG_H

// qtext/commentview.h
// 10/9/2011

#include <QWebView>

namespace QtExt {

class WebDialog: public QWebView
{
  Q_OBJECT
  typedef WebDialog Self;
  typedef QWebView Base;

public:
  explicit WebDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
};

} // namespace QtExt

#endif // _QTEXT_WEBDIALOG_H
