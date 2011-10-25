#ifndef WEBBROWSER_H
#define WEBBROWSER_H

// webbrowser.h
// 7/10/2011

#include "webbrowser_config.h"
#include <QMainWindow>

namespace Ui { class WebBrowserUi; }

class WebBrowser : public QMainWindow
{
  Q_OBJECT
  typedef WebBrowser Self;
  typedef QMainWindow Base;
  typedef Ui::WebBrowserUi Form;

public:
  WEBBROWSER_EXPORT explicit WebBrowser(QWidget *parent = 0);
  WEBBROWSER_EXPORT ~WebBrowser();

  WEBBROWSER_EXPORT void newTab(const QString &url);

  // - Properties -
public:
  WEBBROWSER_EXPORT int tabCount() const;
  WEBBROWSER_EXPORT const QString &homePage() const;
  WEBBROWSER_EXPORT const QString &searchEngine() const;

public slots:
  WEBBROWSER_EXPORT void setHomePage(const QString &url);
  WEBBROWSER_EXPORT void setSearchEngine(const QString &url);

  // - Actions -
public slots:
  WEBBROWSER_EXPORT void closeTab(int index);
  WEBBROWSER_EXPORT void newTab();
  WEBBROWSER_EXPORT void openUrl();
  WEBBROWSER_EXPORT void openSearch();
  WEBBROWSER_EXPORT void reload();
  WEBBROWSER_EXPORT void updateAddressbar();
  WEBBROWSER_EXPORT void location();
  WEBBROWSER_EXPORT void back();
  WEBBROWSER_EXPORT void forward();
  WEBBROWSER_EXPORT void updateButtons();
  WEBBROWSER_EXPORT void handleLoadStarted();
  WEBBROWSER_EXPORT void handleLoadFinished();
  WEBBROWSER_EXPORT void toggleFullScreen();

private:
  Form *ui_;

  QString homePage_;
  QString searchEngine_;
};

#endif // WEBBROWSER_H
