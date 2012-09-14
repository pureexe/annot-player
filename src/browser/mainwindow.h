#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 3/14/2012

#include "lib/webbrowser/webbrowser.h"
#include <QtCore/QTimer>

QT_FORWARD_DECLARE_CLASS(QPropertyAnimation)

class AcPlayer;
class AcTranslator;
class AcDownloader;
class AcConsole;
class FadeAnimation;
class Magnifier;
class AnnotationDownloader;

class MainWindow: public WebBrowser
{
  Q_OBJECT
  Q_PROPERTY(qreal windowOpacity READ windowOpacity WRITE setWindowOpacity)
  Q_DISABLE_COPY(MainWindow)
  typedef MainWindow Self;
  typedef WebBrowser Base;

  AcPlayer *playerDelegate_;
  AcDownloader *downloaderDelegate_;
  AcTranslator *translatorDelegate_;
  AcConsole * console_;
  AnnotationDownloader *annotationDownloader_;
  Magnifier *magnifier_;

  QTimer *autoHideToolBarTimer_;

  FadeAnimation *fadeAni_;

public:
  explicit MainWindow(QWidget *parent = nullptr);

public slots:
  void quit() override;
  void login();
  void newWindow();
protected:
  QStringList startupUrls();
  void saveRecentTabs();

protected slots:
  void openUrlWithAcDownloader(const QString &url);
  void openUrlWithAcPlayer(const QString &url);
  void importUrlToAcPlayer(const QString &url);
  void downloadAnnotationUrl(const QString &url);

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
  void setVisible(bool visible) override;
protected:
  bool event(QEvent *e) override;
  void keyPressEvent(QKeyEvent *e) override;
  void closeEvent(QCloseEvent *e) override;
  void focusInEvent(QFocusEvent *e) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;

  bool isGlobalPosAroundToolBar(const QPoint &pos) const;

  bool isValidWindowSize(const QSize &sz) const;

protected slots:
  void showConsole();
  void toggleMagnifier();
  void autoHideToolBar();

  void translateText(const QString &text);

protected slots:
  void notifyFileSaved(const QString &fileName);

private:
  void createMenus();
  void createGestures();
};

#endif // MAINWINDOW_H
