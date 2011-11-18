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
  WEBBROWSER_API explicit WebBrowser(QWidget *parent = 0);
  WEBBROWSER_API ~WebBrowser();

  WEBBROWSER_API void newTab(const QString &url);

  WEBBROWSER_API qreal textSizeMultiplier() const;

  // - Properties -
public:
  WEBBROWSER_API int tabCount() const;
  WEBBROWSER_API const QString &homePage() const;
  WEBBROWSER_API const QString &searchEngine() const;

public slots:
  WEBBROWSER_API void setHomePage(const QString &url);
  WEBBROWSER_API void setSearchEngine(const QString &url);
  WEBBROWSER_API void setTextSizeMultiplier(qreal factor);

  // - Actions -
public slots:
  WEBBROWSER_API void closeTab(int index);
  WEBBROWSER_API void newTab();
  WEBBROWSER_API void openUrl();
  WEBBROWSER_API void openSearch();
  WEBBROWSER_API void reload();
  WEBBROWSER_API void updateAddressbar();
  WEBBROWSER_API void location();
  WEBBROWSER_API void back();
  WEBBROWSER_API void forward();
  WEBBROWSER_API void updateButtons();
  WEBBROWSER_API void handleLoadStarted();
  WEBBROWSER_API void handleLoadFinished();
  WEBBROWSER_API void toggleFullScreen();

private:
  static QString tidyUrl(const QString &address);

private:
  Form *ui_;

  QString homePage_;
  QString searchEngine_;
  qreal textSizeMultiplier_;
};

#endif // WEBBROWSER_H
