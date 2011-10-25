#ifndef _CORE_GUI_WEBDIALOG_H
#define _CORE_GUI_WEBDIALOG_H

// commentview.h
// 10/9/2011

#include <QWebView>

namespace Core { namespace Gui {

class WebDialog: public QWebView
{
  Q_OBJECT
  typedef WebDialog Self;
  typedef QWebView Base;

public:
  explicit WebDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
};

} } // namespace Gui, Core

#endif // _CORE_GUI_WEBDIALOG_H
