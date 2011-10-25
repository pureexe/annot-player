#ifndef TRENDVIEW_H
#define TRENDVIEW_H

// trendview.h
// 7/15/2011
// TODO: make this a tabbed browser?

#include <module/webbrowser/webbrowser.h>

class TrendView: public WebBrowser
{
  Q_OBJECT
  typedef TrendView Self;
  typedef WebBrowser Base;

public:
  explicit TrendView(QWidget *parent = 0);

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
};

#endif // TRENDVIEW_H
