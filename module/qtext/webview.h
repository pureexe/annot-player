#ifndef _QTEXT_WEBVIEW_H
#define _QTEXT_WEBVIEW_H

// qtext/webview.h
// 10/9/2011

#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QAction)

namespace QtExt {

class WebView: public QWebView
{
  Q_OBJECT
  typedef WebView Self;
  typedef QWebView Base;

  bool loading_;

public:
  explicit WebView(QWidget *parent = 0);
private:
  void createActions();

  //-  Log -
signals:
  void openLinkRequested(const QString &url);
  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);

public:
  QString hoveredLink() const;

  bool isFinished() const { return !loading_; }
  bool isLoading() const { return loading_; }

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
  void goToRecent(int i);
  void invalidteSelection();

  void openWithOperatingSystem();

  virtual void showLink(const QString &url, const QString &title, const QString &content);

  void updateHoveredLink() { hoveredLink_ = hoveredLink(); }

protected:
  static QString shortenText(const QString &text, int len = 40);
  virtual void contextMenuEvent(QContextMenuEvent *e); ///< \override
  virtual void wheelEvent(QWheelEvent *e); ///< \override

  static QString fileNameFromUrl(const QUrl &url, const QString &suffix = QString());

  QMenu *createContextMenu();
  QMenu *createHistoryMenu();

protected:
  QAction *clipAct,
          *clearHighlightAct,
          *openWithOperatingSystemAct,
          *zoomInAct,
          *zoomOutAct,
          *zoomResetAct;
  QString hoveredLink_;
};

} // namespace QtExt

#endif // _QTEXT_WEBVIEW_H
