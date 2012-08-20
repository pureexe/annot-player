#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 2/17/2012

#include "project/common/acmainwindow.h"
#include "module/searchengine/searchenginefactory.h"
#include <QtCore/QList>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAction;
class QComboBox;
//class QGestureEvent;
class QMenu;
class QMenuBar;
class QSortFilterProxyModel;
class QStandardItemModel;
class QTimer;
class QToolButton;
QT_END_NAMESPACE

struct DownloadTaskInfo;
class AcAbout;
class AcBrowser;
class AcDownloaderServer;
class AcFilteredListView;
class AcPlayer;
class AcPreferences;
class ClipboardMonitor;
class DownloadManager;
class DownloadTask;
class MrlResolver;
class Signer;
class TaskDialog;
class TrayIcon;

class MainWindow : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)
  typedef MainWindow Self;
  typedef AcMainWindow Base;

protected:
  enum HeaderData {
    HD_Name = 0,
    HD_State,
    HD_Size,
    HD_Speed,
    HD_Percentage,
    HD_RemainingTime,
    HD_Path,
    HD_Url,
    HD_Id,
    HD_Count
  };

  // - Construction -
public:
  explicit MainWindow(QWidget *parent = nullptr);
  bool isAddingUrls() const;

//signals:
//  void downloadFinished(const QString &path, const QString &url);

public slots:
  void clear();
  void quit();
  //void removeCurrent();

  void checkClipboard();

  void addUrls(const QStringList &urls, bool annotOnly = false);
  void addUrl(const QString &url, bool annotOnly = false);
  void addTask(DownloadTask *t);
  void addTask(const DownloadTaskInfo &t);

  void promptUrl(const QString &text);
  void promptUrls(const QStringList &urls);

  void setVisible(bool visible) override;
  void openDirectory();

  // - Implementations -
protected:
  static void setHeaderData(QAbstractItemModel *model);
  int currentId() const;
  QString currentTitle() const;
  DownloadTask *currentTask() const;
  QString currentFile() const;

protected slots:
  void openLocation(const QString &path);
  void about();
  void preferences();
  void add();
  void start();
  void restart();
  void startAll();
  void stopAll();
  void removeAll();
  void stop();
  void remove();
  void play();
  void browse();
  void save();
  void saveLater();

  void refresh();
  void updateButtons();
  void updateActions();
  void updateCopyMenu();

  void finish(DownloadTask *task);

  void copyTitle();
  void copyUrl();

protected:
  QString downloadStateToString(int state) const;
  QString downloadSizeToString(qint64 size) const;
  QString downloadSpeedToString(qreal speed) const;
  QString downloadTimeToString(qint64 msec) const;

  TaskDialog *taskDialog();

protected slots:
  void searchWithEngine(int engine, const QString &key);
  void searchCurrentTitleWithGoogle() { searchWithEngine(SearchEngineFactory::Google, currentTitle()); }
  void searchCurrentTitleWithGoogleImages() { searchWithEngine(SearchEngineFactory::GoogleImages, currentTitle()); }
  void searchCurrentTitleWithBing()   { searchWithEngine(SearchEngineFactory::Bing, currentTitle()); }
  void searchCurrentTitleWithNicovideo() { searchWithEngine(SearchEngineFactory::Nicovideo, currentTitle()); }
  void searchCurrentTitleWithBilibili() { searchWithEngine(SearchEngineFactory::Bilibili, currentTitle()); }
  void searchCurrentTitleWithAcfun()  { searchWithEngine(SearchEngineFactory::Acfun, currentTitle()); }
  void searchCurrentTitleWithYoutube() { searchWithEngine(SearchEngineFactory::Youtube, currentTitle()); }
  void searchCurrentTitleWithYouku()  { searchWithEngine(SearchEngineFactory::Youku, currentTitle()); }
  void searchCurrentTitleWithWikiEn() { searchWithEngine(SearchEngineFactory::WikiEn, currentTitle()); }
  void searchCurrentTitleWithWikiJa() { searchWithEngine(SearchEngineFactory::WikiJa, currentTitle()); }
  void searchCurrentTitleWithWikiZh() { searchWithEngine(SearchEngineFactory::WikiZh, currentTitle()); }

  // - Events -
public:
  bool event(QEvent *event) override;
protected:
  void contextMenuEvent(QContextMenuEvent *e) override;
  void keyPressEvent(QKeyEvent *e) override;
  void closeEvent(QCloseEvent *e) override;

  //void gestureEvent(QGestureEvent *e);

  // - Logs -
protected slots:
  void notifyFileSaved(const QString &fileName);

private:
  void createModels();
  void createLayout();
  void createSearchEngines();
  void createActions();
private:
  bool disposed_;
  AcAbout *about_;
  AcPreferences *preferences_;
  AcDownloaderServer *appServer_;
  AcBrowser *browserDelegate_;
  AcPlayer *playerDelegate_;
  TrayIcon *trayIcon_;
  DownloadManager *downloadManager_;
  ClipboardMonitor *clipboardMonitor_;
  Signer *signer_;

  QList<SearchEngine *> searchEngines_;

  TaskDialog *taskDialog_;
  AcFilteredListView *tableView_;
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;

  QToolButton *startButton_,
              *stopButton_,
              *removeButton_,
              *addButton_,
              *playButton_,
              *browseButton_,
              *openDirectoryButton_;

  QTimer *refreshTimer_;
  QTimer *saveTimer_;

  QMenuBar *menuBar_;
  QMenu *contextMenu_;
  QMenu *copyMenu_;
  QAction *newAct_,
          *menuBarAct_,
          *startAct_,
          *stopAct_,
          *restartAct_,
          *startAllAct_,
          *stopAllAct_,
          *removeAllAct_,
          *removeAct_,
          *playAct_,
          *browseAct_,
          *openDirectoryAct_;

  QAction *toggleWindowOnTopAct_;

  QAction *copyTitleAct_,
          *copyUrlAct_;

  QAction *quitAct_,
          *hideAct_;
};

#endif // MAINWINDOW_H
