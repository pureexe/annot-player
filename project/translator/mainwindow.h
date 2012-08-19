#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 8/13/2012

#include "project/common/acmainwindow.h"
#include "module/searchengine/searchenginefactory.h"
#include <QtCore/QList>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QMenu;
class QMenuBar;
class QToolButton;
class QUrl;
QT_END_NAMESPACE

class AcTranslatorServer;
class AcBrowser;
class AcDownloader;
class TrayIcon;
class TranslateEdit;
class TranslateBrowser;

class Translator;
class TranslatorManager;

class MainWindow : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)
  typedef MainWindow Self;
  typedef AcMainWindow Base;

  enum Language {
    English = 0,
    TraditionalChinese,
    SimplifiedChinese,
    Korean,
    French,
    German,
    Italian,
    Spanish,
    Portuguese,
    Russian,
    Japanese,
    LanguageCount
  };

  enum {
    TextBrowser = 0,
    DictBrowser0,
    DictBrowser1,
    DictBrowser2,
    DictBrowser3,
    BrowserCount
  };
  TranslateBrowser *browsers_[BrowserCount];

  Translator *dictTranslator_;
  TranslatorManager *textTranslator_;

  TranslateEdit *translateEdit_;
  QToolButton *translateButton_;

  QComboBox *languageCombo_;

  QToolButton *autoButton_, *clipboardButton_, *topButton_;
  QCheckBox *jdicButton_, *romajiButton_, *microsoftButton_, *googleButton_;

  QString languageCode_;

public:
  explicit MainWindow(QWidget *parent = nullptr);

  // - Properties -
protected slots:
  void updateTranslators();
  void updateLanguage();

  // - Events -
public slots:
  void setVisible(bool visible) override;
protected:
  void contextMenuEvent(QContextMenuEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

  // - Actions -

public slots:
  void translateClipboard();
  void showTranslate(const QString &text);
  void showTranslate(const QStringList &l);
protected slots:
  void translate();
  void translate(const QString &text);
  void saveSettings();

  void highlightText(const QString &text);

  void autoTranslate();
  void autoTranslate(const QString &text);

  void refresh();
  void quit();
  void showPreferences();
  void showAbout();

protected slots:
  void searchWithEngine(int engine, const QString &key);
  //void searchCurrentTitleWithGoogle() { searchWithEngine(SearchEngineFactory::Google, currentTitle()); }
  //void searchCurrentTitleWithGoogleImages() { searchWithEngine(SearchEngineFactory::GoogleImages, currentTitle()); }
  //void searchCurrentTitleWithBing()   { searchWithEngine(SearchEngineFactory::Bing, currentTitle()); }
  //void searchCurrentTitleWithNicovideo() { searchWithEngine(SearchEngineFactory::Nicovideo, currentTitle()); }
  //void searchCurrentTitleWithBilibili() { searchWithEngine(SearchEngineFactory::Bilibili, currentTitle()); }
  //void searchCurrentTitleWithAcfun()  { searchWithEngine(SearchEngineFactory::Acfun, currentTitle()); }
  //void searchCurrentTitleWithYoutube() { searchWithEngine(SearchEngineFactory::Youtube, currentTitle()); }
  //void searchCurrentTitleWithYouku()  { searchWithEngine(SearchEngineFactory::Youku, currentTitle()); }
  //void searchCurrentTitleWithWikiEn() { searchWithEngine(SearchEngineFactory::WikiEn, currentTitle()); }
  //void searchCurrentTitleWithWikiJa() { searchWithEngine(SearchEngineFactory::WikiJa, currentTitle()); }
  //void searchCurrentTitleWithWikiZh() { searchWithEngine(SearchEngineFactory::WikiZh, currentTitle()); }

protected slots:
  void showGoogleTranslation(const QString &text);
  void showMicrosoftTranslation(const QString &text);
  void showRomajiTranslation(const QString &text);

  void showDictTranslation(const QString &text);
  void showTextTranslation(const QString &text);

  void openUrl(const QUrl &url);

private:
  void createLayout();
  void createActions();
  void createSearchEngines();

private:
  bool disposed_;
  AcTranslatorServer *appServer_;
  AcBrowser *browserDelegate_;

  TrayIcon *trayIcon_;

  QList<SearchEngine *> searchEngines_;
  QMenuBar *menuBar_;
  QMenu *contextMenu_;
};

#endif // MAINWINDOW_H
