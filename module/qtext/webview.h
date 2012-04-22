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

protected slots:
  void download(const QNetworkRequest &req);
  void clip();
  void zoomIn();
  void zoomOut();
  void zoomReset();

  void setLoading();
  void setFinished();

  virtual void showLink(const QString &url);

protected:
  virtual void contextMenuEvent(QContextMenuEvent *e); ///< \override
  virtual void wheelEvent(QWheelEvent *e); ///< \override

  static QString fileNameFromUrl(const QUrl &url, const QString &suffix = QString());

  QMenu *createContextMenu();
protected:
  QAction *clipAct,
          *zoomInAct,
          *zoomOutAct,
          *zoomResetAct;
};

} // namespace QtExt

#endif // _QTEXT_WEBVIEW_H
