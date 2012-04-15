#ifndef WEBBROWSER_H
#define WEBBROWSER_H

// webbrowser.h
// 7/10/2011

#include "webbrowser_config.h"
#include <QMainWindow>
#include <QUrl>
#include <QList>
#include <QNetworkRequest>

QT_BEGIN_NAMESPACE
class QTimer;
class QNetworkCookieJar;
class QNetworkAccessManager;
class QWebPage;
class QWebView;
QT_END_NAMESPACE

namespace Ui { class WebBrowserUi; }

class WebBrowser : public QMainWindow
{
  Q_OBJECT
  typedef WebBrowser Self;
  typedef QMainWindow Base;
  typedef Ui::WebBrowserUi Form;

public:
  WB_API explicit WebBrowser(QWidget *parent = 0);
  WB_API ~WebBrowser();

signals:
  void openUrlWithAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);

  // - Properties -
public:
  WB_API QString searchEngine() const { return searchEngine_; }
  WB_API QString homePage() const { return homePage_; }
  WB_API QStringList homePages() const { return homePages_; }
  WB_API QList<QUrl> closedUrls() const { return closedUrls_; }
  WB_API qreal textSizeMultiplier() const { return textSizeMultiplier_; }
  WB_API QNetworkCookieJar *cookieJar() { return cookieJar_; }
  WB_API QList<QUrl> blockedUrls() { return blockedUrls_; }

public slots:
  WB_API void setSearchEngine(const QString &url) { searchEngine_ = url; }
  WB_API void setHomePage(const QString &url) { homePage_ = url; }
  WB_API void setHomePages(const QStringList &urls);
  WB_API void setBlockedUrls(const QList<QUrl> &urls) { blockedUrls_ = urls; }
  WB_API void setClosedUrls(const QList<QUrl> &urls) { closedUrls_ = urls; }
  WB_API void setTextSizeMultiplier(qreal factor) { textSizeMultiplier_ = factor; }
  WB_API void setCookieJar(QNetworkCookieJar *cookieJar) { cookieJar_ = cookieJar; }

  // - Actions -
public slots:
  WB_API void showMessage(const QString &text);
  WB_API void error(const QString &text);
  WB_API void warn(const QString &text);
  WB_API void notify(const QString &text);

  WB_API void openUrl(const QString &url);
  WB_API void openUrls(const QStringList &urls);

  WB_API void addRecentUrl(const QString &url);
  WB_API void addRecentUrls(const QStringList &urls);

protected slots:
  WB_API void closeTab(int index);
  WB_API void closeTab();
  WB_API void undoCloseTab();
  WB_API void newTab(QWebView *w = 0);
  WB_API void newTabWithDefaultPage();
  WB_API void openUrl();
  WB_API void search();
  WB_API void search(const QString &text);
  WB_API void reload();
  WB_API void updateAddressbar();
  WB_API void back();
  WB_API void forward();
  WB_API void previousTab();
  WB_API void nextTab();
  WB_API void updateButtons();
  WB_API void handleLoadStarted();
  WB_API void handleLoadFinished();
  WB_API void toggleFullScreen();

  WB_API void focusLocationBar();
  WB_API void focusSearchBar();
  WB_API void focusTab(int index);

  WB_API void focusTab0() { focusTab(0); }
  WB_API void focusTab1() { focusTab(1); }
  WB_API void focusTab2() { focusTab(2); }
  WB_API void focusTab3() { focusTab(3); }
  WB_API void focusTab4() { focusTab(4); }
  WB_API void focusTab5() { focusTab(5); }
  WB_API void focusTab6() { focusTab(6); }
  WB_API void focusTab7() { focusTab(7); }
  WB_API void focusTab8() { focusTab(8); }
  WB_API void focusLastTab() { focusTab(tabCount() - 1); }

protected:
  WB_API int tabCount() const;
  WB_API QUrl tabUrl(int index) const;
  WB_API QList<QUrl> tabUrls() const;
  WB_API QString searchAddress(const QString &text) const;
  WB_API QString tabAddress(int index) const;
  WB_API QStringList tabAddresses() const;
  WB_API QStringList recentUrls() const;

  QNetworkAccessManager *makeNetworkAccessManager();
  void setupWebPage(QWebPage *page);

protected:
  WB_API virtual void closeEvent(QCloseEvent *event); ///< \override

protected:
  WB_API QString completeUrl(const QString &address) const;
  static QString tidyUrl(const QString &address);

  static QString decodeUrl(const QUrl &url); ///< real url to visible address
  static QUrl encodeUrl(const QString &url); ///< visible address to real url

  // - Implementations -
private:
  void setupUi();
  void createActions();

private:
  Form *ui_;
  QNetworkCookieJar *cookieJar_;
  QTimer *hideStatusBarTimer_;
  QList<QUrl> closedUrls_;
  QList<QUrl> blockedUrls_;

  QString homePage_;
  QStringList homePages_;
  QString searchEngine_;
  qreal textSizeMultiplier_;
};

#endif // WEBBROWSER_H
