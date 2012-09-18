#ifndef QXWEBVIEW_H
#define QXWEBVIEW_H

// qxwebview.h
// 10/9/2011

#include "qtx/qtxglobal.h"
#include <QtCore/QString>
#include <QtNetwork/QNetworkRequest>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE
class QAction;
class QGestureEvent;
class QMenu;
class QPanGesture;
class QPinchGesture;
class QSwipeGesture;
QT_END_NAMESPACE

QTX_BEGIN_NAMESPACE

class QxWebView: public QWebView
{
  Q_OBJECT
  Q_DISABLE_COPY(QxWebView)
  typedef QxWebView Self;
  typedef QWebView Base;

  // - Construction -
public:
  explicit QxWebView(QWidget *parent = nullptr);
private:
  void createActions();

signals:
  void menuBarVisibilityChangeRequested(bool visible);
  void openLinkRequested(const QString &url);
  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);

  //-  Properties -
public:
  QString hoveredLink() const;

  int progress() const { return progress_; }
  bool isLoading() const { return progress_ < 100; }
  bool isFinished() const { return progress_ == 100; }

  // - Actions -
public slots:
  void clip();
  void zoomIn();
  void zoomOut();
  void zoomReset();
  void clearHighlight();

  void scrollTop();
  void scrollBottom();
  void scrollLeft();
  void scrollRight();
  void scroll(int dx, int dy, const QRect &r) { Base::scroll(dx, dy, r); }

protected slots:
  void resetStyleSheet() { setStyleSheet(QString()); }

  void download(const QNetworkRequest &req);

  void setLoading();
  void setFinished();
  void updateProgress(int progress) { progress_ = progress; }
  void goToRecent(int i);
  void invalidteSelection();

  void openWithOperatingSystem();

  virtual void showLink(const QString &url, const QString &title, const QString &content);

  void updateHoveredLink() { hoveredLink_ = hoveredLink(); }

  // - Events -
public:
  bool event(QEvent *e) override;
  void gestureEvent(QGestureEvent *e);
protected:
  void contextMenuEvent(QContextMenuEvent *e) override;
  void wheelEvent(QWheelEvent *e) override;

  void panGesture(QPanGesture *g);
  void pinchGesture(QPinchGesture *g);
  void swipeGesture(QSwipeGesture *g);

  QMenu *createHistoryMenu();

  // - Helpers -
protected:
  static QString shortenText(const QString &text, int len = 40);
  static QString fileNameFromUrl(const QUrl &url, const QString &suffix = QString());

protected:
  int progress_;
  QAction *clipAct,
          *clearHighlightAct,
          *openWithOperatingSystemAct,
          *zoomInAct,
          *zoomOutAct,
          *zoomResetAct;
  QString hoveredLink_;
};

QTX_END_NAMESPACE

#endif // QXWEBVIEW_H
