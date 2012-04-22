#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 3/14/2012

#include <module/webbrowser/webbrowser.h>

class AcPlayer;
class AcDownloader;

class MainWindow: public WebBrowser
{
  Q_OBJECT
  typedef MainWindow Self;
  typedef WebBrowser Base;

  AcPlayer *playerDelegate_;
  AcDownloader *downloaderDelegate_;

public:
  explicit MainWindow(QWidget *parent = 0);

public slots:
  void login();
  void newWindow();
  virtual void showMessage(const QString &text); ///< \override
protected:
  QStringList startupUrls();
  void saveRecentTabs();

protected slots:
  void openUrlWithAcDownloader(const QString &url);
  void openUrlWithAcPlayer(const QString &url);
  void importUrlToAcPlayer(const QString &url);

  void loadCookieJar();
  void saveCookieJar();

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
protected:
  virtual bool event(QEvent *event); ///< \override
  virtual void closeEvent(QCloseEvent *event); ///< \override
};

#endif // MAINWINDOW_H
