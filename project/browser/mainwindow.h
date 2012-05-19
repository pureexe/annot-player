#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 3/14/2012

#include "module/webbrowser/webbrowser.h"
#include <QtCore/QTimer>

QT_FORWARD_DECLARE_CLASS(QPropertyAnimation)

class AcPlayer;
class AcDownloader;
class AcConsole;
class FadeAnimation;
class Magnifier;

class MainWindow: public WebBrowser
{
  Q_OBJECT
  Q_PROPERTY(qreal windowOpacity READ windowOpacity WRITE setWindowOpacity)
  Q_DISABLE_COPY(MainWindow)
  typedef MainWindow Self;
  typedef WebBrowser Base;

  AcPlayer *playerDelegate_;
  AcDownloader *downloaderDelegate_;
  AcConsole * console_;
  Magnifier *magnifier_;

  QTimer *autoHideToolBarTimer_;

  FadeAnimation *fadeAni_;

public:
  explicit MainWindow(QWidget *parent = 0);

public slots:
  virtual void quit(); ///< \override
  void login();
  void newWindow();
protected:
  QStringList startupUrls();
  void saveRecentTabs();

protected slots:
  void openUrlWithAcDownloader(const QString &url);
  void openUrlWithAcPlayer(const QString &url);
  void importUrlToAcPlayer(const QString &url);

  void loadCookieJar();
  void saveCookieJar();

  void about();
  void preferences();

  void clip();
  void reload();
  void stop();
  void zoomIn();
  void zoomOut();
  void zoomReset();
  void inspect();
  void scrollTop();
  void scrollBottom();
  void toggleFullScreen() { setFullScreen(!isFullScreen()); }
  void setFullScreen(bool t);

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
protected:
  virtual bool event(QEvent *e); ///< \override
  virtual void keyPressEvent(QKeyEvent *e); ///< \override
  virtual void closeEvent(QCloseEvent *e); ///< \override
  virtual void focusInEvent(QFocusEvent *e); ///< \override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \override
  virtual void mousePressEvent(QMouseEvent *event); ///< \override

  bool isGlobalPosAroundToolBar(const QPoint &pos) const;

protected slots:
  void showConsole();
  void toggleMagnifier();
  void autoHideToolBar();


  // - Helpers -
  bool isValidWindowSize(const QSize &sz) const;

private:
  void createMenus();
  void createGestures();
};

#endif // MAINWINDOW_H
