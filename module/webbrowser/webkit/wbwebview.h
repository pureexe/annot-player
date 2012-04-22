#ifndef WBWEBVIEW_H
#define WBWEBVIEW_H

// wbwebview.h
// 1/27/2012

#include "module/qtext/webview.h"

typedef QtExt::WebView WbWebViewBase;
class WbWebView : public WbWebViewBase
{
  Q_OBJECT
  typedef WbWebView Self;
  typedef WbWebViewBase Base;

public:
  explicit WbWebView(QWidget *parent = 0);

signals:
  void windowCreated(QWebView *w);
  void openUrlWithAcPlayerRequested(const QString &url);
  void importUrlToAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);
  void undoClosedTabRequested();

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *e); ///< \override
  virtual QWebView *createWindow(QWebPage::WebWindowType type); ///< \override

private slots:
  void openWithAcPlayer()
  { emit openUrlWithAcPlayerRequested(currentUrl_); }

  void importToAcPlayer()
  { emit importUrlToAcPlayerRequested(currentUrl_); }

  void openWithAcDownloader()
  { emit openUrlWithAcDownloaderRequested(currentUrl_); }

private:
  void createActions();
private:
  QAction *openWithAcPlayerAct_,
          *importToAcPlayerAct_,
          *openWithAcDownloaderAct_,
          *undoClosedTabAct_;
  QString currentUrl_;
};

#endif // WBWEBVIEW_H
