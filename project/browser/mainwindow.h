#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 3/14/2012

#include <module/webbrowser/webbrowser.h>

class MainWindow: public WebBrowser
{
  Q_OBJECT
  typedef MainWindow Self;
  typedef WebBrowser Base;

public:
  explicit MainWindow(QWidget *parent = 0);

public slots:
  void login();
  void newWindow();
protected:
  QStringList startupUrls();
  void saveRecentUrls();

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
protected:
  virtual bool event(QEvent *event); ///< \override
  virtual void closeEvent(QCloseEvent *event); ///< \override
};

#endif // MAINWINDOW_H
