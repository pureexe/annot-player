#ifdef USE_MODULE_WEBBROWSER
#ifndef CLOUDVIEW_H
#define CLOUDVIEW_H


// cloudview.h
// 7/15/2011
// TODO: make this a tabbed browser?

#include <module/webbrowser/webbrowser.h>

class CloudView: public WebBrowser
{
  Q_OBJECT
  typedef CloudView Self;
  typedef WebBrowser Base;

public:
  explicit CloudView(QWidget *parent = 0);

  //virtual void setVisible(bool visible); ///< \override
};

#endif // CLOUDVIEW_H
#endif // USE_MODULE_WEBBROWSER
