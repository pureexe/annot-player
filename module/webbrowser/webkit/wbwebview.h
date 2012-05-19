#ifndef WBWEBVIEW_H
#define WBWEBVIEW_H

// webkit/wbwebview.h
// 1/27/2012

#include "module/qtext/webview.h"
#include <QtCore/QObjectList>

QT_FORWARD_DECLARE_CLASS(QMenu)

class SearchEngine;

typedef QtExt::WebView WbWebViewBase;
class WbWebView : public WbWebViewBase
{
  Q_OBJECT
  Q_DISABLE_COPY(WbWebView)
  typedef WbWebView Self;
  typedef WbWebViewBase Base;

public:
  explicit WbWebView(QWidget *parent = 0);
  ~WbWebView();

signals:
  void windowCreated(QWebView *w);
  void openUrlWithAcPlayerRequested(const QString &url);
  void importUrlToAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);
  void undoClosedTabRequested();
  void newWindowRequested();
  void fullScreenRequested();
  void toggleMenuBarVisibleRequested();

  void searchWithEngineRequested(const QString &text, int engine);

public:
  bool hasSearchEngines() const { return !searchEngines_.isEmpty(); }

public slots:
  void setSearchEngine(int engine) { searchEngine_ = engine; }
  void setSearchEngines(const QList<SearchEngine *> &l) { searchEngines_ = l; }

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *e); ///< \override
  virtual QWebView *createWindow(QWebPage::WebWindowType type); ///< \override

  QString selectedUrl() const;

protected slots:
  void updateTitle();

  void openSelectedLink()
  { emit openLinkRequested(selectedUrl()); }

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
  QMenu *contextMenu_; // FIXME
  QObjectList deleteLater_;
  QAction *openWithAcPlayerAct_,
          *importToAcPlayerAct_,
          *openWithAcDownloaderAct_,
          *undoClosedTabAct_,
          *openSelectedLinkAct_,
          *newWindowAct_,
          *fullScreenAct_,
          *menuBarAct_;
  QString currentUrl_;

  int searchEngine_;
  QList<SearchEngine *> searchEngines_;
};

#endif // WBWEBVIEW_H
