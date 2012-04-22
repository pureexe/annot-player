#ifndef WEBBROWSER_H
#define WEBBROWSER_H

// webbrowser.h
// 7/10/2011

#include "webbrowser_config.h"
#include <QtGui/QMainWindow>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>
#include <QtCore/QList>

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
  explicit WebBrowser(QWidget *parent = 0);
  ~WebBrowser();

signals:
  void openUrlWithAcPlayerRequested(const QString &url);
  void importUrlToAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);

  // - Properties -
public:
  QString searchEngine() const { return searchEngine_; }
  QString homePage() const { return homePage_; }
  QStringList homePages() const { return homePages_; }
  QList<QUrl> closedUrls() const { return closedUrls_; }
  qreal textSizeMultiplier() const { return textSizeMultiplier_; }
  QNetworkCookieJar *cookieJar() { return cookieJar_; }
  QList<QUrl> blockedUrls() { return blockedUrls_; }

public slots:
  void setSearchEngine(const QString &url) { searchEngine_ = url; }
  void setHomePage(const QString &url) { homePage_ = url; }
  void setHomePages(const QStringList &urls);
  void setBlockedUrls(const QList<QUrl> &urls) { blockedUrls_ = urls; }
  void setClosedUrls(const QList<QUrl> &urls) { closedUrls_ = urls; }
  void setTextSizeMultiplier(qreal factor) { textSizeMultiplier_ = factor; }
  void setCookieJar(QNetworkCookieJar *cookieJar) { cookieJar_ = cookieJar; }

  // - Actions -
public slots:
  virtual void showMessage(const QString &text);
  void error(const QString &text);
  void warn(const QString &text);
  void notify(const QString &text);

  void openUrl(const QString &url);
  void openUrls(const QStringList &urls);
  void newTab(const QString &url);

  void addRecentUrl(const QString &url);
  void addRecentUrls(const QStringList &urls);

protected slots:
  void closeTab(int index);
  void closeTab();
  void undoCloseTab();
  void newTab(QWebView *w = 0, int index = -1);
  void newTabAfterCurrent();
  void newTabWithDefaultPage();
  void openUrl();
  void search();
  void search(const QString &text);
  void reload();
  void updateAddressbar();
  void back();
  void forward();
  void previousTab();
  void nextTab();
  void updateButtons();
  void handleLoadStarted();
  void handleLoadFinished();
  void toggleFullScreen();

  void focusLocationBar();
  void focusSearchBar();
  void focusTab(int index);

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

protected:
  int tabCount() const;
  int tabIndex() const;
  QUrl tabUrl(int index) const;
  QList<QUrl> tabUrls() const;
  QString searchAddress(const QString &text) const;
  QString tabAddress(int index) const;
  QStringList tabAddresses() const;
  QStringList recentUrls() const;

  QNetworkAccessManager *networkAccessManager();

protected:
  virtual void closeEvent(QCloseEvent *event); ///< \override

protected:
  QString completeUrl(const QString &address) const;
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
  QNetworkAccessManager *nam_;
  QTimer *hideStatusBarTimer_;
  QList<QUrl> closedUrls_;
  QList<QUrl> blockedUrls_;

  QString homePage_;
  QString cacheDirectory_;
  QStringList homePages_;
  QString searchEngine_;
  qreal textSizeMultiplier_;
};

#endif // WEBBROWSER_H
