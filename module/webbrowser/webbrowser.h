#ifndef WEBBROWSER_H
#define WEBBROWSER_H

// webbrowser.h
// 7/10/2011

#include "module/qtext/draggablemainwindow.h"
#include <QtNetwork/QNetworkRequest>
#include <QtGui/QWidgetList>
#include <QtCore/QList>
#include <QtCore/QUrl>

#define WB_BLANK_PAGE   "about:blank"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkCookieJar;
class QTimer;
class QWebPage;
class QWebView;
QT_END_NAMESPACE

namespace Ui { class WebBrowserUi; }

class SearchEngine;
class MouseGesture;
class MouseGestureFilter;

typedef QtExt::DraggableMainWindow WebBrowserBase;
class WebBrowser : public WebBrowserBase
{
  Q_OBJECT
  Q_DISABLE_COPY(WebBrowser)
  typedef WebBrowser Self;
  typedef WebBrowserBase Base;
  typedef Ui::WebBrowserUi Form;

public:
  explicit WebBrowser(QWidget *parent = 0);
  ~WebBrowser();

signals:
  void searchEngineChanged(int);
  void openUrlWithAcPlayerRequested(const QString &url);
  void importUrlToAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);
  void newWindowRequested();
  void fullScreenRequested();

  // - Properties -
public:
  QString homePage() const { return homePage_; }
  QStringList homePages() const { return homePages_; }
  QList<QUrl> closedUrls() const { return closedUrls_; }
  qreal textSizeMultiplier() const { return textSizeMultiplier_; }
  QNetworkCookieJar *cookieJar() { return cookieJar_; }
  QList<QUrl> blockedUrls() { return blockedUrls_; }

public slots:
  virtual void quit() { close(); }
  void invalidateSearch();
  void setHomePage(const QString &url) { homePage_ = url; }
  void setHomePages(const QStringList &urls);
  void setBlockedUrls(const QList<QUrl> &urls) { blockedUrls_ = urls; }
  void setClosedUrls(const QList<QUrl> &urls) { closedUrls_ = urls; }
  void setTextSizeMultiplier(qreal factor) { textSizeMultiplier_ = factor; }
  void setCookieJar(QNetworkCookieJar *cookieJar) { cookieJar_ = cookieJar; }

  // - Actions -
public slots:
  void showMessage(const QString &text);
  void error(const QString &text);
  void warn(const QString &text);
  void notify(const QString &text);

  void openUrl(const QString &url, QWebView *view = 0);
  void openBlankPage();
  void openUrls(const QStringList &urls);
  void newTab(const QString &url);
  void newTabInBackground(const QString &url);

  void addRecentUrl(const QString &url);
  void addRecentUrl(const QUrl &url) { addRecentUrl(url.toString()); }
  void addRecentUrls(const QStringList &urls);

  void addRecentSearch(const QString &text);
  void addRecentSearches(const QStringList &l);

protected slots:
  void openUrlWithOperatingSystem(const QString &url);
  void openLinkInNewTab();
  void closeTab(int index);
  void closeTab();
  void undoCloseTab();
  void newTab(QWebView *w = 0, int index = -1, bool focus = true);
  void newTabAfterCurrent(bool focus = true);
  void newTabAfterCurrentWithBlankPage();
  void newTabAtLastWithBlankPage();
  void search(const QString &text, int engine = -1);
  void searchInNewTab(const QString &text, int engine = -1);
  void reload();
  void updateWindowTitle();
  void updateAddressbar();
  void back();
  void forward();
  void previousTab();
  void nextTab();
  void updateButtons();
  void handleLoadStarted();
  void handleLoadFinished();
  void toggleFullScreen();
  void updateLoadProgress();
  void showLoadProgress();

  void focusLocationBar();
  void focusSearchBar();
  void focusTab(int index);
  void setSearchEngine(int engine);
  void setSearchedText(const QString &t);

  void focusTab0() { focusTab(0); }
  void focusTab1() { focusTab(1); }
  void focusTab2() { focusTab(2); }
  void focusTab3() { focusTab(3); }
  void focusTab4() { focusTab(4); }
  void focusTab5() { focusTab(5); }
  void focusTab6() { focusTab(6); }
  void focusTab7() { focusTab(7); }
  void focusTab8() { focusTab(8); }
  void focusLastTab() { focusTab(tabCount() - 1); }

  void setCacheDirectory(const QString &fileName) { cacheDirectory_ = fileName; }

  void setToolBarVisible(bool visible);
  bool isToolBarUnderMouse() const;
  bool toolBarHasFocus() const;

  void setTabBarVisible(bool visible);
  bool isTabBarUnderMouse() const;
  bool tabBarHasFocus() const;
  void toggleMenuBarVisible();

protected:
  bool isTabBarVisible() const;

  int tabCount() const;
  int tabIndex() const;
  QUrl tabUrl(int index) const;
  QList<QUrl> tabUrls() const;
  QString searchAddress(const QString &text, int engine = -1) const;
  QString tabAddress(int index) const;
  QStringList tabAddresses() const;
  QStringList recentUrls() const;
  QStringList recentSearches() const;

  QWidget *tabWidget() const;
  QWidget *tabWidget(int i) const;
  QString tabTitle() const { return tabTitle(tabIndex()); }
  QString tabTitle(int i) const;

  int searchEngine() const { return searchEngine_; }

  QNetworkAccessManager *networkAccessManager();

  void addMouseGesture(MouseGesture *g);

  // - Events -
protected:
  virtual void closeEvent(QCloseEvent *event); ///< \override

protected:
  QString completeUrl(const QString &address) const;
  int searchEngineForAddress(const QString &address) const;
  static QString tidyUrl(const QString &address);

  static QString decodeUrl(const QUrl &url); ///< real url to visible address
  static QUrl encodeUrl(const QString &url); ///< visible address to real url

  // - Implementations -
private:
  void setupUi();
  void createActions();
  void createSearchEngines();
  void createGestures();

private:
  Form *ui;
  QNetworkCookieJar *cookieJar_;
  QNetworkAccessManager *nam_;
  MouseGestureFilter *mouseGestureFilter_;
  QTimer *hideStatusBarTimer_;
  QList<QUrl> closedUrls_;
  QList<QUrl> blockedUrls_;

  QWidgetList toolBarWidgets_;

  QList<SearchEngine *> searchEngines_;
  int searchEngine_;

  QString homePage_;
  QString cacheDirectory_;
  QStringList homePages_;
  qreal textSizeMultiplier_;
  int loadProgress_;
};

#endif // WEBBROWSER_H
