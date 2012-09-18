#ifndef WBWEBVIEW_H
#define WBWEBVIEW_H

// webkit/wbwebview.h
// 1/27/2012

#include "qtx/qxwebview.h"

class SearchEngine;

typedef QxWebView WbWebViewBase;
class WbWebView : public WbWebViewBase
{
  Q_OBJECT
  Q_DISABLE_COPY(WbWebView)
  typedef WbWebView Self;
  typedef WbWebViewBase Base;

public:
  explicit WbWebView(QWidget *parent = nullptr);

signals:
  void windowCreated(QWebView *w);
  void openUrlWithAcPlayerRequested(const QString &url);
  void importUrlToAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);
  void downloadAnnotationUrlRequested(const QString &url);
  void undoClosedTabRequested();
  void newWindowRequested();
  void fullScreenRequested();
  void toggleMenuBarVisibleRequested();

  void searchWithEngineRequested(const QString &text, int engine);

  void selectedTextChanged(QString);

public:
  bool hasSearchEngines() const { return !searchEngines_.isEmpty(); }

public slots:
  void setSearchEngine(int engine) { searchEngine_ = engine; }
  void setSearchEngines(const QList<SearchEngine *> &l) { searchEngines_ = l; }

  // - Events -
protected:
  void contextMenuEvent(QContextMenuEvent *e) override;
  QWebView *createWindow(QWebPage::WebWindowType type) override;

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

  void downloadAnnotations()
  { emit downloadAnnotationUrlRequested(currentUrl_); }

  void searchWithEngine(int engine);
  void invalidateSelection();

  void showLink(const QString &url, const QString &title, const QString &content) override;
  void highlightDownloadableLink(const QString &link, const QString &title, const QString &content);

private:
  void createActions();
private:
  QAction *openWithAcPlayerAct_,
          *importToAcPlayerAct_,
          *openWithAcDownloaderAct_,
          *downloadAnnotationsAct_,
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
