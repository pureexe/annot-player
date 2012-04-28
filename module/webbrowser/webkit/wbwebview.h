#ifndef WBWEBVIEW_H
#define WBWEBVIEW_H

// webkit/wbwebview.h
// 1/27/2012

#include "module/qtext/webview.h"

class WbSearchEngine;

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
  void newWindowRequested();
  void fullScreenRequested();

  void searchWithEngineRequested(const QString &text, int engine);

public:
  bool hasSearchEngines() const { return !searchEngines_.isEmpty(); }

public slots:
  void setSearchEngine(int engine) { searchEngine_ = engine; }
  void setSearchEngines(const QList<WbSearchEngine *> &l) { searchEngines_ = l; }

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *e); ///< \override
  virtual QWebView *createWindow(QWebPage::WebWindowType type); ///< \override

  QString selectedUrl() const;

protected slots:
  void openWithAcPlayer()
  { emit openUrlWithAcPlayerRequested(currentUrl_); }

  void importToAcPlayer()
  { emit importUrlToAcPlayerRequested(currentUrl_); }

  void openWithAcDownloader()
  { emit openUrlWithAcDownloaderRequested(currentUrl_); }

  void searchWithEngine(int engine);
  void invalidateSelection();

  virtual void showLink(const QString &url, const QString &title, const QString &content); ///< \override
  void highlightDownloadableLink(const QString &link, const QString &title, const QString &content);

private:
  void createActions();
private:
  QAction *openWithAcPlayerAct_,
          *importToAcPlayerAct_,
          *openWithAcDownloaderAct_,
          *undoClosedTabAct_,
          *newWindowAct_,
          *fullScreenAct_;
  QString currentUrl_;

  int searchEngine_;
  QList<WbSearchEngine *> searchEngines_;
};

#endif // WBWEBVIEW_H
