#ifndef WEBBROWSER_H
#define WEBBROWSER_H

// webbrowser.h
// 7/10/2011

#include "webbrowser_config.h"
#include <QMainWindow>
#include <QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QWebPage)

namespace Ui { class WebBrowserUi; }

class WebBrowser : public QMainWindow
{
  Q_OBJECT
  typedef WebBrowser Self;
  typedef QMainWindow Base;
  typedef Ui::WebBrowserUi Form;

public:
  WEBBROWSER_API explicit WebBrowser(QWidget *parent = 0);
  WEBBROWSER_API ~WebBrowser();

  // - Properties -
public:
  WEBBROWSER_API QString homePage() const { return homePage_; }
  WEBBROWSER_API QString searchEngine() const { return searchEngine_; }
  WEBBROWSER_API qreal textSizeMultiplier() const { return textSizeMultiplier_; }
  WEBBROWSER_API QNetworkCookieJar *cookieJar() { return cookieJar_; }

public slots:
  WEBBROWSER_API void setHomePage(const QString &url) { homePage_ = url; }
  WEBBROWSER_API void setSearchEngine(const QString &url) { searchEngine_ = url; }
  WEBBROWSER_API void setTextSizeMultiplier(qreal factor) { textSizeMultiplier_ = factor; }
  WEBBROWSER_API void setCookieJar(QNetworkCookieJar *cookieJar) { cookieJar_ = cookieJar; }

  // - Actions -
public slots:
  WEBBROWSER_API void openUrl(const QString &url);
  WEBBROWSER_API void openUrls(const QStringList &urls);

protected slots:
  WEBBROWSER_API void closeTab(int index);
  WEBBROWSER_API void closeTab();
  WEBBROWSER_API void newTab();
  WEBBROWSER_API void openUrl();
  WEBBROWSER_API void openSearch();
  WEBBROWSER_API void reload();
  WEBBROWSER_API void updateAddressbar();
  WEBBROWSER_API void back();
  WEBBROWSER_API void forward();
  WEBBROWSER_API void updateButtons();
  WEBBROWSER_API void handleLoadStarted();
  WEBBROWSER_API void handleLoadFinished();
  WEBBROWSER_API void toggleFullScreen();

  WEBBROWSER_API void focusLocationBar();
  WEBBROWSER_API void focusSearchBar();

protected:
  int tabCount() const;
  QNetworkAccessManager *makeNetworkAccessManager();
  void setupWebPage(QWebPage *page);

  WEBBROWSER_API virtual void closeEvent(QCloseEvent *event); ///< \override

protected:
  static QString decodeUrl(const QUrl &url); ///< real url to visible address
  static QUrl encodeUrl(const QString &url); ///< visible address to real url

  static QString tidyUrl(const QString &address);
  static QString completeUrl(const QString &address);

  // - Implementations -
private:
  void setupUi();
  void createActions();

private:
  Form *ui_;
  QNetworkCookieJar *cookieJar_;

  QString homePage_;
  QString searchEngine_;
  qreal textSizeMultiplier_;
};

#endif // WEBBROWSER_H
